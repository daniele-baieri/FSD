import sys
import os
import cv2


if __name__ == "__main__":


    image_folder = sys.argv[1]
    fps = 50.0
    video_name = 'video.mp4'

    images = [img for img in os.listdir(image_folder) if img.endswith(".png")]
    frame = cv2.imread(os.path.join(image_folder, images[0]))
    height, width, layers = frame.shape

    video = cv2.VideoWriter(os.path.join(image_folder, video_name), -1, fps, (width,height))

    for image in images:
        img = os.path.join(image_folder, image)
        video.write(cv2.imread(img))
        os.remove(img)

    cv2.destroyAllWindows()
    video.release()