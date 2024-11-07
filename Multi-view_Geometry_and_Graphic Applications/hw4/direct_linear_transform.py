import os
import cv2
import numpy as np


def direct_linear_transform(correspondences):
    H = np.eye(3) # Initialize homography by 3x3 identity matrix
    # TODO #2: Complete the coefficient matrix A, where Ah = 0 
    # Note: h is the solution vector of homography, please see slides.
    # 1. The size of the coefficient matrix A should be 2n x 9, where n is the number of correspondences.
    # 2. Each correspondence contributes two rows to A, please see slides. 
    # 3. Report the size of matrix A and the correspondence  
    # 4. Report at least two rows of a correspondence to explain that you are correct to write the correspondence matrix 
    # Hint: Assume w = 1 for the homogeneous representation

    # TODO #3: Obtain the svd of A using np.linalg.svd (use default options)
    # Note: Report the outputs of np.linalg.svd

    # TODO #4: Obtain the unit singular vector with least singular value. Please see the slides and np.linalg.svd doc.
    # Note: Report the unit singular vector.

    # TODO #5: h is the unit singular vector. Update homography H by h (please see slides).
    # Note: 
    # 1. Report H.
    # 2. Report the geometric distance (error in one image) for solved H (please see slides)
    # 3. The reported geometric distance should be averaged by number of correspondences. 
    # 4. Before compute the error, you should scale the result of x'=Hx so that the third component of x' = 1

    return H


# Load test images
reference_image = cv2.imread('images/reference.png')
distorted_image = cv2.imread('images/tilted.png')

# Solve homography H by direct linear transform.
# x' = Hx, where x denotes a point in the reference image, x' denotes a point in the distored image
# TODO #1: assign correspondences: 
# The format: [[[x1, y1], [x1', y1']], [[x2, y2], [x2', y2']], ...] (the former is on reference image)
# Note: 
# 1. At least select 8 correspondences.
# 2. Draw selected points on the images and attach them in your report
correspondences = []
H = direct_linear_transform(correspondences)

# Generate undistorted image by the solved homography
image_h, image_w = reference_image.shape[0], reference_image.shape[1]
undistorted_image = np.zeros([image_h, image_w, 3], dtype=np.uint8)
for x in range(image_w):
    for y in range(image_h):
        reference_point = np.array([x, y, 1])
        distorted_point = np.array([x, y, 1]) #Initialization
        # TODO #6 : compute corresponding point on the distorted image  
        # Note: you should scale the corresponding point so that its third component = 1
        # Note: Attach reference.png, distorted.png, and undistorted.png on the report for visual comparison

        # Assign color of corresponding point in the undistorted image 
        x2, y2 = round(distorted_point[0]), round(distorted_point[1])
        if x2 >= 0 and x2 < image_w and y2 >= 0 and y2 < image_h:
            undistorted_image[y, x, :] = distorted_image[y2, x2, :]

# Bonus: Use image difference between reference/distorted and reference/undistorted and describe the effect of alignment in the report.

# Dump the undistorted image
# Create output folder if not exist
folder_out = "results"
if not os.path.exists(folder_out):
    os.makedirs(folder_out)
# Write image to the folder
cv2.imwrite(folder_out+'/undistorted.png', undistorted_image)
