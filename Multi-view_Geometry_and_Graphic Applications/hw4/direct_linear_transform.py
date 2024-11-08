
# Mouse callback function
def select_points(event, x, y, flags, param):
    global points_reference, points_distorted, num_points, reference_image, combined_image
    img_width = reference_image.shape[1]

    if event == cv2.EVENT_LBUTTONDOWN:
        if x < img_width and len(points_reference) < num_points:
            points_reference.append((x, y))
            print(f"Point {len(points_reference)} selected on Reference Image: {(x, y)}")
            cv2.circle(combined_image, (x, y), 5, (0, 255, 0), -1)
            cv2.putText(combined_image, str(len(points_reference)), (x + 10, y - 10), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)
            cv2.imshow("Images", combined_image)

        elif x >= img_width and len(points_distorted) < num_points:
            points_distorted.append((x - img_width, y))
            print(f"Point {len(points_distorted)} selected on Distorted Image: {(x - img_width, y)}")
            cv2.circle(combined_image, (x, y), 5, (0, 0, 255), -1)
            cv2.putText(combined_image, str(len(points_distorted)), (x + 10, y - 10), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)
            cv2.imshow("Images", combined_image)


def direct_linear_transform(correspondences):
    # TODO #2: Complete the coefficient matrix A, where Ah = 0 
    # Note: h is the solution vector of homography, please see slides.
    # 1. The size of the coefficient matrix A should be 2n x 9, where n is the number of correspondences.
    # 2. Each correspondence contributes two rows to A, please see slides. 
    # 3. Report the size of matrix A and the correspondence  
    # 4. Report at least two rows of a correspondence to explain that you are correct to write the correspondence matrix 
    # Hint: Assume w = 1 for the homogeneous representation
    A = np.zeros((2 * len(correspondences), 9))
    for i, correspondence in enumerate(correspondences):
        x, y = correspondence[0]
        x_prime, y_prime = correspondence[1]

        A[2 * i] = [0, 0, 0, -x, -y, -1, y_prime * x, y_prime * y, y_prime]
        A[2 * i + 1] = [x, y, 1, 0, 0, 0, -x_prime * x, -x_prime * y, -x_prime]

    print(f"Size of matrix A: {A.shape}")
    print(f"First two rows of A for first correspondence:")
    print(A[0])
    print(A[1])

    # TODO #3: Obtain the svd of A using np.linalg.svd (use default options)
    # Note: Report the outputs of np.linalg.svd
    U, S, Vt = np.linalg.svd(A)
    print(f"\nSVD results:")
    print(f"U: {U}")
    print(f"S {S}")
    print(f"Vt: {Vt}")

    # TODO #4: Obtain the unit singular vector with least singular value. Please see the slides and np.linalg.svd doc.
    # Note: Report the unit singular vector.
    h = Vt[-1, :]
    print(f"\nUnit singular vector h:")
    print(h)

    # TODO #5: h is the unit singular vector. Update homography H by h (please see slides).
    # Note: 
    # 1. Report H.
    # 2. Report the geometric distance (error in one image) for solved H (please see slides)
    # 3. The reported geometric distance should be averaged by number of correspondences. 
    # 4. Before compute the error, you should scale the result of x'=Hx so that the third component of x' = 1
    H = h.reshape(3, 3)

    # Compute average geometric error
    total_error = 0
    for corr in correspondences:
        x = np.array([corr[0][0], corr[0][1], 1])
        x_prime_actual = np.array([corr[1][0], corr[1][1], 1])
        x_prime_estimated = H @ x
        x_prime_estimated = x_prime_estimated / x_prime_estimated[2]

        error = (x_prime_actual[0] - x_prime_estimated[0]) ** 2 + (x_prime_actual[1] - x_prime_estimated[1]) ** 2
        total_error += error

    avg_error = total_error / len(correspondences)
    print(f"\nFinal Homography H:")
    print(H)
    print(f"Average geometric error: {avg_error}")

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

# Initialize lists to store points for both images
points_reference = []
points_distorted = []
num_points = 8

# Show Reference and Distorted Images for user to select 8 correspondences
combined_image = np.hstack((reference_image, distorted_image))
img_width = reference_image.shape[1]
cv2.putText(combined_image, 'Reference Image', (250, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2, cv2.LINE_AA)
cv2.putText(combined_image, 'Distorted Image', (img_width + 250, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2, cv2.LINE_AA)
cv2.imshow("Images", combined_image)
cv2.setMouseCallback("Images", select_points)

while len(points_reference) < num_points or len(points_distorted) < num_points:
    cv2.waitKey(1)

cv2.putText(combined_image, "Press 'Space' To Continue", (int(combined_image.shape[1] / 2), int(combined_image.shape[0] - 30)), 
    cv2.FONT_HERSHEY_SIMPLEX, 1, (255,255,255), 2)
cv2.imshow("Images", combined_image)
while True:
    key = cv2.waitKey(0)
    if key == ord(' '):
        break

cv2.destroyWindow("Images")
cv2.waitKey(1)

print("8 points on Reference Image:", points_reference)
print("8 points on Distorted Image:", points_distorted)

# Merge into a single array
correspondences = [[[p1[0], p1[1]], [p2[0], p2[1]]] for p1, p2 in zip(points_reference, points_distorted)]
print("Merged 8 correspondences:", correspondences)

# Calculate H
H = direct_linear_transform(correspondences)

# Generate undistorted image by the solved homography
image_h, image_w = reference_image.shape[0], reference_image.shape[1]
undistorted_image = np.zeros([image_h, image_w, 3], dtype=np.uint8)
for x in range(image_w):
    for y in range(image_h):
        reference_point = np.array([x, y, 1])
        # TODO #6 : compute corresponding point on the distorted image  
        # Note: you should scale the corresponding point so that its third component = 1
        # Note: Attach reference.png, distorted.png, and undistorted.png on the report for visual comparison
        distorted_point = H @ np.array([x, y, 1])
        distorted_point /= distorted_point[2]

        # Assign color of corresponding point in the undistorted image 
        x2, y2 = round(distorted_point[0]), round(distorted_point[1])
        if x2 >= 0 and x2 < image_w and y2 >= 0 and y2 < image_h:
            undistorted_image[y, x, :] = distorted_image[y2, x2, :]

# Bonus: Use image difference between reference/distorted and reference/undistorted and describe the effect of alignment in the report.
reference_distorted_diff = np.clip(np.abs(reference_image.astype(np.int16) - distorted_image.astype(np.int16)), a_min=0, a_max=255).astype(np.uint8)
reference_undistorted_diff = np.clip(np.abs(reference_image.astype(np.int16) - undistorted_image.astype(np.int16)), a_min=0, a_max= 255).astype(np.uint8)

# Dump the undistorted image
# Create output folder if not exist
folder_out = "results"
if not os.path.exists(folder_out):
    os.makedirs(folder_out)
# Write image to the folder
cv2.imwrite(folder_out+'/undistorted.png', undistorted_image)
cv2.imwrite(folder_out+'/reference_distorted_diff.png', reference_distorted_diff)
cv2.imwrite(folder_out+'/reference_undistorted_diff.png', reference_undistorted_diff)

# Show Results
combined_result_image = np.hstack((undistorted_image, reference_distorted_diff, reference_undistorted_diff))
cv2.putText(combined_result_image, 'Undistorted Image', (250, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2, cv2.LINE_AA)
cv2.putText(combined_result_image, 'Reference/Distorted Diff', (img_width + 250, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2, cv2.LINE_AA)
cv2.putText(combined_result_image, 'Reference/Undistorted Diff', (2 * img_width + 250, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2, cv2.LINE_AA)
cv2.imshow("Result", combined_result_image)
cv2.waitKey(0)
