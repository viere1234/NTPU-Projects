import os
import cv2
import numpy as np
import scipy.linalg


class DirectLinearTransformer():
    def __init__(self):
        # Initialize camera_matrix by 3x3 identity matrix
        self.target_matrix = np.array([[1, 0, 0, 0],
                                       [0, 1, 0, 0],
                                       [0, 0, 1, 0]], dtype=float)

    def build_measurement_matrix(self, correspondences):
        # TODO #2: Complete the coefficient matrix A, where A * solution = 0
        # Let the last component = 1 for homogeneous representation of the points
        A = []
        for corr in correspondences:
            X, x = corr[0], corr[1]
            X = np.append(X, 1)
            x = np.append(x, 1)
            A.append([
                0, 0, 0, 0,
                -X[0], -X[1], -X[2], -X[3],
                x[1] * X[0], x[1] * X[1], x[1] * X[2], x[1] * X[3]
            ])
            A.append([
                X[0], X[1], X[2], X[3],
                0, 0, 0, 0,
                -x[0] * X[0], -x[0] * X[1], -x[0] * X[2], -x[0] * X[3]
            ])
        return np.array(A)

    def update_target_matrix(self, solution):
        # TODO #3: Update self.target matrix from the solution vector
        # 1. Should update the matrix by 'num_rows' and 'num_cols', not specific numbers 3 and 4.
        num_rows, num_cols = self.target_matrix.shape[0], self.target_matrix.shape[1]
        self.target_matrix = solution.reshape(num_rows, num_cols)

    def estimation(self, correspondences):
        # TODO #4: Finish estimation pipeline by the following tools
        # 1. self.build_measurement_matrix
        # 2. np.linalg.svd
        # 3. self.update_target_matrix
        A = self.build_measurement_matrix(correspondences)
        _, _, Vt = np.linalg.svd(A)
        solution = Vt[-1]
        self.update_target_matrix(solution)
        return self.target_matrix


def calculate_error(correspondences, target_matrix):
    # TODO #5: Calculate geometric distance (reprojection error, please see slides) 
    # 1. The error should be averaged by number of correspondences. 
    # 2. You should normalized the projected point so that the last component of point is 1
    error = 0
    for corr in correspondences:
        X = np.append(corr[0], 1)
        x = np.append(corr[1], 1)
        x_proj = target_matrix @ X
        x_proj = x_proj / x_proj[2]
        error += (x_proj[0] - x[0]) ** 2 + (x_proj[1] - x[1]) ** 2
    return error / len(correspondences)


# TODO #1: Load 3D to 2D correspondences from the .txt file in the inputs
# File format: X Y Z x y on each line, where (X, Y, Z) is the 3D point, (x, y) is the 2D point. 
# Correspondence format: [[[X1, Y1, Z1], [x1, y1]], [[X2, Y2, Z2], [x2, y2]], ...]
def load_correspondences(filename):
    correspondences = []
    with open(filename, 'r') as f:
        for line in f:
            values = list(map(float, line.strip().split()))
            X = values[:3]
            x = values[3:]
            correspondences.append([X, x])
    return correspondences


if __name__ == "__main__":
    for filename in ['corr.txt', 'corr-subpixel.txt']:
        print(f"\nProcessing {filename}")
        filepath = os.path.join('inputs', filename)
        correspondences = load_correspondences(filepath)

        dlt = DirectLinearTransformer()
        P = dlt.estimation(correspondences)
        error = calculate_error(correspondences, P)
        print(f"Initial error: {error}")

        # TODO #6: Scale P such that p_{31}^2 + p_{32}^2 + p_{33}^2 = 1
        # Report the scaled P and its error in the doc.
        scale = 1.0 / np.sqrt(np.sum(P[2, :3] ** 2))
        P_scaled = P * scale
        error_scaled = calculate_error(correspondences, P_scaled)
        print(f"Scaled error: {error_scaled}")

        # TODO Bonus #1: Use scipy.linalg.rq to perform RQ decomposition on the 'scaled' camera matrix to get the values of calibration matrix K and R
        # 1. Report K, R, K@R for both scaled camera matrices solved by using corr.tex and corr-subpixel.txt in the doc.
        # 2. Should ensure the diagonal entries of K be positive.
        # 3. Provide implementation here.
        M = P_scaled[:, :3]
        K, R = scipy.linalg.rq(M)
        T = np.diag(np.sign(np.diag(K)))
        K = K @ T
        R = T @ R

        print("\nBonus #1 - Decomposition results:")
        print("K:\n", K)
        print("R:\n", R)
        print("K@R:\n", K @ R)

        # TODO Bonus #2: Solve translation t to finalize the decomposition of camera matrix P=K[R|t]
        # 1. Share your ideas in the doc.
        # 2. Provide implementation here.
        t = np.linalg.inv(K) @ P_scaled[:, 3]
        print("\nBonus #2 - Translation vector:")
        print("t:", t)

        # TODO Bonus #3: Restricted camera estimation (please see slides for the following notations)
        # 1. Initialize alpha = alpha_x = alpha_y in calibration matrix K by averaging entries of P
        # 2. Use the decomposed matrix K to directly assign x0, y0
        # 3. Convert the decomposed rotation matrix R to 3-d angle axis vector by cv2.rodrigues and use the vector to represent rotation parameters.
        # 4. Use the decomposed translation t to represent translation parameters
        # 5. Report the initial 9 variable parameters (alpha, x0, y0, rotation, translation) in the doc.
        # 6. Use scipy.optimize.least_squares for refining the 9 variable parameters
        # 7. Report the final 9 variable parameters in the doc.
        # 8. Share the formulations in the doc. and provide implementation here.
        print("\nBonus #3 - Restricted camera estimation:")

        alpha = (K[0, 0] + K[1, 1]) / 2
        x0 = K[0, 2]
        y0 = K[1, 2]
        rot_vec, _ = cv2.Rodrigues(R)

        print("Initial parameters:")
        print(f"alpha = {alpha}")
        print(f"x0, y0 = {x0}, {y0}")
        print(f"rotation = {rot_vec.squeeze()}")
        print(f"translation = {t}")

        params_init = np.concatenate(([alpha], [x0, y0], rot_vec.flat, t.flat))

        def params_to_matrix(params):
            alpha, x0, y0 = params[0], params[1], params[2]
            rot_vec = params[3:6]
            t = params[6:9]
            K = np.array([
                [alpha, 0, x0],
                [0, alpha, y0],
                [0, 0, 1]
            ])
            R, _ = cv2.Rodrigues(rot_vec)
            P = K @ np.hstack([R, t.reshape(3, 1)])
            return P

        def cost_function(params):
            P = params_to_matrix(params)
            errors = []
            for X, x in correspondences:
                X_homo = np.append(X, 1)
                PX = P @ X_homo
                x_homo = np.append(x, 1)
                error1 = x_homo[1] * PX[2] - PX[1]
                error2 = PX[0] - x_homo[0] * PX[2]

                errors.extend([error1, error2])
            return np.array(errors)

        result = scipy.optimize.least_squares(cost_function, params_init, method='lm')

        params_final = result.x
        P_final = params_to_matrix(params_final)

        print("\nFinal parameters:")
        print(f"alpha = {params_final[0]}")
        print(f"x0, y0 = {params_final[1]}, {params_final[2]}")
        print(f"rotation = {params_final[3:6]}")
        print(f"translation = {params_final[6:9]}")
        print(f"Final cost = {result.cost}")

        error_restricted = calculate_error(correspondences, P_final)
        print(f"Restricted camera error: {error_restricted}")
