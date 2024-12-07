import numpy as np
import cv2


class CorrespondencesSelector:
    def __init__(self, firstImageFileName: str, secondImageFileName: str):
        first_image = cv2.imread(firstImageFileName)
        second_image = cv2.imread(secondImageFileName)
        
        first_image = cv2.resize(first_image, None, fx=0.8, fy=0.8, interpolation=cv2.INTER_AREA)
        second_image = cv2.resize(second_image, None, fx=0.8, fy=0.8, interpolation=cv2.INTER_AREA)
        
        self.combined_image = np.hstack((first_image, second_image))
        self.img_width = first_image.shape[1]
        self.points = []
        self.points_prime = []
        self.correspondences = []

    def _mouse_callback(self, event, x, y, flags, param):
        if event == cv2.EVENT_LBUTTONDOWN:
            if x < self.img_width and len(self.points) < 8:
                self.points.append((x, y))
                cv2.circle(self.combined_image, (x, y), 8, (0, 255, 0), -1)
                cv2.putText(self.combined_image, str(len(self.points)),
                            (x + 10, y - 10), cv2.FONT_HERSHEY_SIMPLEX,
                            2, (255, 255, 255), 2)
                cv2.imshow("Select Correspondences", self.combined_image)
            elif x >= self.img_width and len(self.points_prime) < 8:
                self.points_prime.append((x, y))
                cv2.circle(self.combined_image, (x, y), 8, (0, 0, 255), -1)
                cv2.putText(self.combined_image, str(len(self.points_prime)),
                            (x + 10, y - 10), cv2.FONT_HERSHEY_SIMPLEX,
                            2, (255, 255, 255), 2)
                cv2.imshow("Select Correspondences", self.combined_image)

    def select(self):
        cv2.imshow("Select Correspondences", self.combined_image)
        cv2.setMouseCallback("Select Correspondences", self._mouse_callback)
        while len(self.points) < 8 or len(self.points_prime) < 8:
            key = cv2.waitKey(1) & 0xFF
            if key == 27:  # ESC key to exit
                cv2.destroyAllWindows()
                return None
        self.correspondences = np.array([([p1[0], p1[1]], [p2[0] - self.img_width, p2[1]])
                                         for p1, p2 in zip(self.points, self.points_prime)], dtype=np.double)
        cv2.imwrite("results/selected_correspondences.jpeg", self.combined_image)
        cv2.destroyAllWindows()
        cv2.waitKey(1)
        return self.correspondences


def normalize_points(points):
    points = np.array(points, dtype=np.double)
    centroid = np.mean(points, axis=0)
    centered = points - centroid
    mean_dist = np.mean(np.linalg.norm(centered, axis=1))
    scale = np.sqrt(2) / mean_dist
    T = np.array([
        [scale, 0, -scale * centroid[0]],
        [0, scale, -scale * centroid[1]],
        [0, 0, 1]
    ], dtype=np.double)
    normalized_points = centered * scale

    return normalized_points, T


def main():
    # Step 1: Select 8 correspondences.
    selector = CorrespondencesSelector("inputs/first.jpeg",
                                       "inputs/second.jpeg")
    correspondences = selector.select()
    print("Initial correspondences:")
    print(correspondences)

    # Step 2: Normalize correspondences.
    correspondences[:, 0], T = normalize_points(correspondences[:, 0])
    correspondences[:, 1], T_prime = normalize_points(correspondences[:, 1])
    print("Normalized correspondences")
    print(correspondences)

    # Step 3: Find linear solution of F
    A = []
    for correspondence in correspondences:
        x, y = correspondence[0]
        x_prime, y_prime = correspondence[1]
        A.append([x_prime * x, x_prime * y, x_prime, y_prime * x, y_prime * y, y_prime, x, y, 1])
    U, S, Vt = np.linalg.svd(A)
    F = Vt[-1].reshape(3, 3)
    print('F:')
    print(F)

    # Step 4: Constraint enforcement
    U, S, Vt = np.linalg.svd(F)
    S[2] = 0
    F_prime = U @ np.diag(S) @ Vt
    print('F\':')
    print(F_prime)
    print(f"det(F\') = {np.linalg.det(F_prime)}")

    # Step 5: Denormalization
    F_final = T_prime.T @ F_prime @ T
    print('Final F:')
    print(F_final)


if __name__ == '__main__':
    main()
