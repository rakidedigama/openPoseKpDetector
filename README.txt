OPENPOSE KEYPOINT TESTER (Newer Version 12/12/2018)

This program builds on an OpenPose example to extract the labelled keypoints and save them to a log file. It also draws a bounding box rectangle outside the detected keypoints. 

UPDATE : It seems that the openPose API has gone through major changes and the headers included in these files may no longer support the updated openPose API. 

INSTRUCTIONS

* Run program from the OpenPoseBBDetection as root. This is required to read the models.
* Threshold command line arguement implemented. ( look at all the possible command line arguments using -- help)
* Keypoints saved to text file named "keypoints_log" inside Output folder
 
<PATH TO OpenPoseBBDetection>build\OpenPoseKpTester\OpenPoseTest.ext <INPUT FOLDER PATH> <OUTPUT FOLDER PATH> -render_threshold=0.5


