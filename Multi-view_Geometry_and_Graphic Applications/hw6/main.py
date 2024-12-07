import os
import cv2
import numpy as np
from tqdm import tqdm


class CameraCalibrator:
    def __init__(self, filename: str):
        self.K = None
        self.omega = None  # Store IAC
        self.filename = filename
        self.img = cv2.imread(self.filename)
        self.points = []
        self.points_prime = [[0, 0], [1, 0], [0, 1], [1, 1]]

    def _mouse_callback(self, event, x, y, flags, param):
        if event == cv2.EVENT_LBUTTONDOWN:
            self.points.append([x, y])
            if len(self.points) <= 4:
                cv2.circle(self.img, (x, y), 5, (0, 255, 0), -1)
            elif len(self.points) <= 8:
                cv2.circle(self.img, (x, y), 5, (255, 0, 0), -1)
            else:
                cv2.circle(self.img, (x, y), 5, (0, 0, 255), -1)
            cv2.imshow('Select Points', self.img)

    @staticmethod
    def _solve_linalg(A):
        _, _, Vt = np.linalg.svd(A)
        solution = Vt[-1]
        return solution.reshape(3, 3)

    def _get_homography(self, points):
        A = []
        for point, point_prime in zip(points, self.points_prime):
            x, y = point_prime
            x_prime, y_prime = point
            A.append([0, 0, 0, -x, -y, -1, y_prime * x, y_prime * y, y_prime])
            A.append([x, y, 1, 0, 0, 0, -x_prime * x, -x_prime * y, -x_prime])

        A = np.array(A)
        H = self._solve_linalg(A)
        return H / H[2, 2]

    def select_points(self):
        cv2.imshow("Select Points", self.img)
        cv2.setMouseCallback('Select Points', self._mouse_callback)
        while len(self.points) < 12:
            cv2.waitKey(1)
        cv2.imwrite("results/metrix_planes_selected_points.jpeg", self.img)
        cv2.destroyWindow('Select Points')
        cv2.waitKey(1)
        print("Select Points for Camera Calibrating:")
        print(self.points)

    def calibrate_camera(self):
        # Solve omega
        homographies = [self._get_homography(self.points[i * 4:(i + 1) * 4]) for i in range(3)]
        A = []
        for H in homographies:
            h1, h2 = H[:, 0], H[:, 1]
            # h1^T ω h2 = 0
            A.append([
                h1[0] * h2[0],                  # ω11
                h1[0] * h2[1] + h1[1] * h2[0],  # ω12, ω21
                h1[0] * h2[2] + h1[2] * h2[0],  # ω13, ω31
                h1[1] * h2[1],                  # ω22
                h1[1] * h2[2] + h1[2] * h2[1],  # ω23, ω32
                h1[2] * h2[2]                   # ω33
            ])
            # h1^T ω h1 = h2^T ω h2
            A.append([
                h1[0] * h1[0] - h2[0] * h2[0],        # ω11
                2 * (h1[0] * h1[1] - h2[0] * h2[1]),  # ω12, ω21
                2 * (h1[0] * h1[2] - h2[0] * h2[2]),  # ω13, ω31
                h1[1] * h1[1] - h2[1] * h2[1],        # ω22
                2 * (h1[1] * h1[2] - h2[1] * h2[2]),  # ω23, ω32
                h1[2] * h1[2] - h2[2] * h2[2]         # ω33
            ])
        _, _, Vt = np.linalg.svd(A)
        omega_vector = Vt[-1]
        self.omega = np.array([[omega_vector[0], omega_vector[1], omega_vector[2]],
                               [omega_vector[1], omega_vector[3], omega_vector[4]],
                               [omega_vector[2], omega_vector[4], omega_vector[5]]])
        print("Omega:")
        print(self.omega)
        # Ensure omega is positive definite
        eigenvalues, eigenvectors = np.linalg.eigh(self.omega)
        if np.any(eigenvalues <= 0):
            eigenvalues = np.abs(eigenvalues)
            self.omega = eigenvectors @ np.diag(eigenvalues) @ eigenvectors.T
            print("New Omega (corrected to positive definite):")
            print(self.omega)

        # Solve K with Cholesky decomposition (diagonal will be positive ensured by Cholesky decomposition)
        L = np.linalg.cholesky(self.omega)
        self.K = np.linalg.inv(L.T)
        self.K = self.K / self.K[2, 2]  # Ensure K33 = 1
        print("K: ")
        print(self.K)
        return self.K


class PlaneOrientationEstimator:
    def __init__(self, K, image):
        self.normal = None
        self.img = image.copy()
        self.vanish_line = []
        self.current_line_points = []
        self.lines = []
        self.selecting_line = False
        self.K = K

    def _mouse_callback(self, event, x, y, flags, param):
        if event == cv2.EVENT_LBUTTONDOWN:
            # Start line selection
            if not self.selecting_line:
                self.current_line_points = [(x, y)]
                self.selecting_line = True
            # Complete line selection
            else:
                self.current_line_points.append((x, y))
                self.lines.append(self.current_line_points[:])
                self.selecting_line = False
                self.current_line_points = []

                # Draw permanent line
                cv2.line(self.img, self.lines[-1][0], self.lines[-1][1],(0, 255, 0),2)
        elif event == cv2.EVENT_MOUSEMOVE and self.selecting_line:
            image_temp = self.img.copy()
            cv2.line(image_temp,
                     self.current_line_points[0],
                     (x, y),
                     (0, 0, 255),
                     2)
            cv2.imshow('Select Parallel Lines', image_temp)

    def _get_vanish_point(self):
        x1, y1 = self.lines[0][0]
        x2, y2 = self.lines[0][1]
        line1 = np.array([y2 - y1, x1 - x2, x2 * y1 - x1 * y2])
        x1, y1 = self.lines[1][0]
        x2, y2 = self.lines[1][1]
        line2 = np.array([y2 - y1, x1 - x2, x2 * y1 - x1 * y2])
        vanish_point = np.cross(line1, line2)
        vanish_point = (vanish_point / vanish_point[2])[:2]
        return vanish_point

    def select_parallel_lines(self):
        cv2.imshow('Select Parallel Lines', self.img)
        cv2.setMouseCallback('Select Parallel Lines', self._mouse_callback)

        vanish_points = []
        while len(vanish_points) < 2:
            cv2.waitKey(1)

            if len(self.lines) > 1:
                vanish_points.append(self._get_vanish_point())
                print("Vanish point:", vanish_points[-1])
                self.lines = []
        cv2.imwrite("results/eecs_selected_lines.jpeg", self.img)
        cv2.destroyWindow('Select Parallel Lines')
        cv2.waitKey(1)

        # Compute vanish line
        x1, y1 = vanish_points[0]
        x2, y2 = vanish_points[1]
        self.vanish_line = np.array([y2 - y1, x1 - x2, x2 * y1 - x1 * y2])
        self.vanish_line = self.vanish_line / self.vanish_line[2]
        print("Vanish line:", self.vanish_line)

    def estimate(self):
        self.normal = np.array(np.transpose(self.K) @ self.vanish_line)
        self.normal = self.normal / np.linalg.norm(self.normal)
        print("Ground plane orientation:", self.normal)


class ImageRectifier:
    def __init__(self, K: np.ndarray, normal: np.ndarray, image):
        self.K = K
        self.normal = np.array(normal, dtype=np.float64)
        self.img = image.copy()
        self.R = None
        self.H = None
        self.rectified_image = None

    def compute_rotation_matrix(self):  # https://math.stackexchange.com/questions/180418/calculate-rotation-matrix-to-align-vector-a-to-vector-b-in-3d
        target = np.array([0, 0, 1], dtype=np.float64)
        v = np.cross(self.normal, target)
        s = np.linalg.norm(v)
        c = np.dot(self.normal, target)
        v_x = np.array([[0, -v[2], v[1]],
                        [v[2], 0, -v[0]],
                        [-v[1], v[0], 0]])
        self.R = np.eye(3) + v_x + (v_x @ v_x) * (1 - c) / (s * s)
        print("Rotation matrix:")
        print(self.R)

    def compute_homography(self):
        self.H = self.K @ self.R @ np.linalg.inv(self.K)
        print("Homography matrix:")
        print(self.H)

    def rectify_image(self):
        image_h, image_w = self.img.shape[0], self.img.shape[1]
        self.rectified_image = np.zeros([image_h, image_w, 3], dtype=np.uint8)
        rectified_points = []
        original_points = []
        for x in tqdm(range(image_w)):
            for y in range(image_h):
                rectified_point = self.H @ np.array([x, y, 1])
                rectified_point /= rectified_point[2]
                rectified_points.append([rectified_point[0], rectified_point[1]])
                original_points.append([x, y])

        diff = np.mean(rectified_points, axis=0) - [image_w / 2, image_h / 2]
        maxs = np.max(rectified_points, axis=0)
        mins = np.min(rectified_points, axis=0)
        scales = [image_w / (maxs[0] - mins[0]), image_h / (maxs[1] - mins[1])]
        for rectified_point, original_point in tqdm(zip(rectified_points, original_points)):
            x2 = round((rectified_point[0] - diff[0] - image_w / 2) * scales[0] + image_w / 2)
            y2 = round((rectified_point[1] - diff[1] - image_h / 2) * scales[1] + image_h / 2)
            x = original_point[0]
            y = original_point[1]
            if 0 <= x2 < image_w and 0 <= y2 < image_h:
                self.rectified_image[y2, x2, :] = self.img[y, x, :]

        if not os.path.exists("results"):
            os.makedirs("results")
        cv2.imwrite("results/rectified_image.jpeg", self.rectified_image)
        cv2.imshow('Rectified Image', self.rectified_image)
        cv2.waitKey(0)


def main():
    # Step 1: Calibrate your own camera by metric planes
    calibrator = CameraCalibrator("inputs/metric_planes.jpeg")
    calibrator.select_points()
    K = calibrator.calibrate_camera()

    # EECS Image
    image = cv2.imread("inputs/eecs.jpeg")

    # Step 2: Estimate ground plane orientation
    planeOrientationEstimator = PlaneOrientationEstimator(K, image)
    planeOrientationEstimator.select_parallel_lines()
    planeOrientationEstimator.estimate()
    n = planeOrientationEstimator.normal

    # Step 3: Rectify the image
    rectifier = ImageRectifier(K, n, image)
    rectifier.compute_rotation_matrix()
    rectifier.compute_homography()
    rectifier.rectify_image()


if __name__ == '__main__':
    main()
