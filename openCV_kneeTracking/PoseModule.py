import cv2
import mediapipe as mp
import time
import matplotlib.pyplot as plt


class PoseDetector:

    def __init__(self, mode=False, upBody=False, smooth=True, detectionCon=0.5, trackCon=0.5):
        self.mode = mode
        self.upBody = upBody
        self.smooth = smooth
        self.detectionCon = detectionCon
        self.trackCon = trackCon

        self.mpDraw = mp.solutions.drawing_utils
        self.mpPose = mp.solutions.pose
        self.pose = self.mpPose.Pose(self.mode, self.upBody, self.smooth, self.detectionCon, self.trackCon)

    def findPose(self, img, draw=True):
        imgRGB = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)  # Mediapipe uses RGB convention
        self.results = self.pose.process(imgRGB)
        if self.results.pose_landmarks:
            if draw:
                self.mpDraw.draw_landmarks(img, self.results.pose_landmarks, self.mpPose.POSE_CONNECTIONS)
        return img

    def findPosition(self, img, draw=True):
        lmList = []
        if self.results.pose_landmarks:
            for idx, lm in enumerate(self.results.pose_landmarks.landmark):
                h, w, c = img.shape
                # print(idx, lm)
                cx, cy = int(lm.x * w), int(lm.y * h)
                lmList.append([idx, cx, cy])
                if draw:
                    cv2.circle(img, (cx, cy), 5, (255, 0, 0), cv2.FILLED)
        return lmList


def main():
    cap = cv2.VideoCapture(1)  # Get frames from webcam
    width = 1920
    height = 1080
    cap.set(3, width)
    cap.set(4, height)
    pTime = 0
    detector = PoseDetector()

    xData_leftKnee = []
    yData_leftKnee = []
    xData_rightKnee = []
    yData_rightKnee = []
    lm_leftKnee = 25
    lm_rightKnee = 26

    while True:
        success, img = cap.read()
        img = detector.findPose(img)
        lmList = detector.findPosition(img, draw=False)
        if len(lmList) != 0:
            # print(lmList)
            print(lmList[14])
            cv2.circle(img, (lmList[lm_leftKnee][1], lmList[lm_leftKnee][2]), 10, (0, 0, 255), cv2.FILLED)
            cv2.circle(img, (lmList[lm_rightKnee][1], lmList[lm_rightKnee][2]), 10, (255, 0, 0), cv2.FILLED)

        cTime = time.time()
        fps = 1/(cTime-pTime)
        pTime = cTime

        cv2.putText(img, str(int(fps)), (70, 50), cv2.FONT_HERSHEY_PLAIN, 3, (255, 0, 0), 3)

        cv2.imshow("Image", img)

        if len(lmList) != 0:
            xData_leftKnee.append(width-lmList[lm_leftKnee][1])
            yData_leftKnee.append(height-lmList[lm_leftKnee][2])
            xData_rightKnee.append(width - lmList[lm_rightKnee][1])
            yData_rightKnee.append(height - lmList[lm_rightKnee][2])

        if cv2.waitKey(1) == ord('q'):
            break

    f = plt.figure()
    ax = plt.axes()
    ax.plot(xData_leftKnee, yData_leftKnee, color='red', marker='x', markersize=5, label="left knee")
    ax.plot(xData_rightKnee, yData_rightKnee, color='blue', marker='x', markersize=5, label="right knee")
    ax.legend()
    plt.title("Body Joints Trajectory")
    plt.xlabel("x [px]")
    plt.ylabel("y [px]")
    plt.grid()
    plt.draw()
    f.savefig("joint_trajectory.pdf", bbox_inches='tight')
    plt.show()


if __name__ == "__main__":
    main()
