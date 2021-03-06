// ------------------------- OpenPose Library Tutorial - Pose - Example 1 - Extract from Image -------------------------
// This first example shows the user how to:
    // 1. Load an image (`filestream` module)
    // 2. Extract the pose of that image (`pose` module)
    // 3. Render the pose on a resized copy of the input image (`pose` module)
    // 4. Display the rendered pose (`gui` module)
// In addition to the previous OpenPose modules, we also need to use:
    // 1. `core` module: for the Array<float> class that the `pose` module needs
    // 2. `utilities` module: for the error & logging functions, i.e. op::error & op::log respectively

// 3rdparty dependencies
// GFlags: DEFINE_bool, _int32, _int64, _uint64, _double, _string
#include "QCoreApplication"
#include <gflags/gflags.h>
// Allow Google Flags in Ubuntu 14
#ifndef GFLAGS_GFLAGS_H_
    namespace gflags = google;
#endif
// OpenPose dependencies
#include <openpose/core/headers.hpp>
#include <openpose/filestream/headers.hpp>
#include <openpose/gui/headers.hpp>
#include <openpose/pose/headers.hpp>
#include <openpose/utilities/headers.hpp>
#include <openpose/wrapper/headers.hpp>

#include <openpose/utilities/keypoint.hpp>
#include "iostream"
#include "QFileInfoList"
#include "QDir"

#include <thread>
#include "json.h"
#include "keypointdetector.h"


// See all the available parameter options withe the `--help` flag. E.g. `build/examples/openpose/openpose.bin --help`
// Note: This command will show you flags for other unnecessary 3rdparty files. Check only the flags for the OpenPose
// executable. E.g. for `openpose.bin`, look for `Flags from examples/openpose/openpose.cpp:`.
// Debugging/Other
DEFINE_int32(logging_level,             3,              "The logging level. Integer in the range [0, 255]. 0 will output any log() message, while"
                                                        " 255 will not output any. Current OpenPose library messages are in the range 0-4: 1 for"
                                                        " low priority messages and 4 for important ones.");
// Producer
//DEFINE_string(image_path,               "examples/media/COCO_val2014_000000000192.jpg",     "Process the desired image.");
// OpenPose
DEFINE_string(model_pose,               "COCO",      "Model to be used. E.g. `COCO` (18 keypoints), `MPI` (15 keypoints, ~10% faster), "
                                                        "`MPI_4_layers` (15 keypoints, even faster but less accurate).");
DEFINE_string(model_folder,             "models/",      "Folder path (absolute or relative) where the models (pose, face, ...) are located.");
DEFINE_string(net_resolution,           "-1x368",       "Multiples of 16. If it is increased, the accuracy potentially increases. If it is"
                                                        " decreased, the speed increases. For maximum speed-accuracy balance, it should keep the"
                                                        " closest aspect ratio possible to the images or videos to be processed. Using `-1` in"
                                                        " any of the dimensions, OP will choose the optimal aspect ratio depending on the user's"
                                                        " input value. E.g. the default `-1x368` is equivalent to `656x368` in 16:9 resolutions,"
                                                        " e.g. full HD (1980x1080) and HD (1280x720) resolutions.");
DEFINE_string(output_resolution,        "-1x-1",        "The image resolution (display and output). Use \"-1x-1\" to force the program to use the"
                                                        " input image resolution.");
DEFINE_int32(num_gpu_start,             0,              "GPU device start number.");
DEFINE_double(scale_gap,                0.3,            "Scale gap between scales. No effect unless scale_number > 1. Initial scale is always 1."
                                                        " If you want to change the initial scale, you actually want to multiply the"
                                                        " `net_resolution` by your desired initial scale.");
DEFINE_int32(scale_number,              1,              "Number of scales to average.");
// OpenPose Rendering
DEFINE_bool(disable_blending,           false,          "If enabled, it will render the results (keypoint skeletons or heatmaps) on a black"
                                                        " background, instead of being rendered into the original image. Related: `part_to_show`,"
                                                        " `alpha_pose`, and `alpha_pose`.");
DEFINE_double(render_threshold,         0.05,           "Only estimated keypoints whose score confidences are higher than this threshold will be"
                                                        " rendered. Generally, a high threshold (> 0.5) will only render very clear body parts;"
                                                        " while small thresholds (~0.1) will also output guessed and occluded keypoints, but also"
                                                        " more false positives (i.e. wrong detections).");
DEFINE_double(alpha_pose,               0.6,            "Blending factor (range 0-1) for the body part rendering. 1 will show it completely, 0 will"
                                                        " hide it. Only valid for GPU rendering.");


DEFINE_string(image_dir,                "",             "Process a directory of images. Use `examples/media/` for our default example folder with 20"
                                                        " images. Read all standard formats (jpg, png, bmp, etc.).");



const bool bDrawRectangle = false;
//bool bWriteImage = true;

void openPoseTutorialPose1(QString,QString);






/*
 * Input arguments : Array of pose keypoints, string vector of keypoint names eg : LShoulder, Nose etc,
 * */
void processKeyPoints(op::Array<float> poseKeypoints,std::string imageName,std::string imgPath, std::string dest_path){
    try{

         std::ofstream log;
         if(!poseKeypoints.empty()){

             imageName =imageName.substr(0, imageName.size()-4);
             std::string logfilename =  dest_path + "\\" + imageName + "_annos.json";
             std::cout <<  "size " << poseKeypoints.printSize() << std::endl;

            int numberOfHumans = poseKeypoints.getSize(0);
            int numberOfJoints = poseKeypoints.getSize(1); // per person
            int keypointDims = poseKeypoints.getSize(2); // x, y and height

            // std::cout << "Keypoint size   0 : " << poseKeypoints.getSize(0) <<  "Keypoint size 1 : " << poseKeypoints.getSize(1) <<  "Keypoint size 2 : " << poseKeypoints.getSize(2) << std::endl;
             std::cout << "Saving keypoints to : " << logfilename << std::endl;

             log.open(logfilename, std::ios::app);
             std::string keypointdata;

             // PRINT KEYPOINTS
             Json::Value root;
             root["imagepath"] = imgPath;
             root["scaleratio"] = 1;
             Json::Value keypoints(Json::arrayValue); // keep empty
             root["keypoints"] = keypoints;

             Json::Value joints(Json::arrayValue); // person, jointlocations.

             for(int humanNumber = 0; humanNumber < numberOfHumans ; humanNumber++){

                 Json::Value jointPerson;
                 Json::Value jointLocations(Json::arrayValue);

                for(int kp = 0; kp < 13  ; kp++){ // 13 > ignore face keypoints ears & eyes (14,15,16,17)

                    float *xpoint;
                    float *ypoint;
                    float *zpoint;
                    if(kp==0){

                        xpoint = poseKeypoints.getPtr() + (humanNumber * numberOfJoints * keypointDims ) +  (kp*3 + 0);
                       ypoint = poseKeypoints.getPtr() + (humanNumber * numberOfJoints * keypointDims ) +  (kp*3 + 1);
                        zpoint = poseKeypoints.getPtr() + (humanNumber * numberOfJoints * keypointDims ) +  (kp*3 + 2);


//                        keypoint["joint_name"] = COCO_KEYPOINT_NAMES[kp];
//                        keypoint["id"] = kp;
//                        keypoint["points"]["x"] = (int)*xpoint;
//                        keypoint["points"]["y"] = (int)*ypoint;
                    }

                    if(kp>0){


                            xpoint = poseKeypoints.getPtr() + (humanNumber * numberOfJoints * keypointDims ) +  ((kp+1)*3 + 0);  // +1 to offset neck kp
                            ypoint = poseKeypoints.getPtr() + (humanNumber * numberOfJoints * keypointDims ) +  ((kp+1)*3 + 1); // Neck Kp is not an original COCO kp. It is
                            zpoint = poseKeypoints.getPtr() + (humanNumber * numberOfJoints * keypointDims ) +  ((kp+1)*3 + 2);  // obtained by OP calculating mid-point of Left
                                                                                                                                        //  & Right Shoulders

                    }


                    int x = (int)*xpoint;
                    int y = (int)*ypoint;
                    if(x!=0 || y!=0) {// found kp, both equals zero if kp not found
                        Json::Value keypoint;
                        keypoint["joint_name"] = COCO_KEYPOINT_NAMES[kp];
                        keypoint["id"] = kp;
                        keypoint["points"]["x"] = x;
                        keypoint["points"]["y"] = y;
                        jointLocations.append(keypoint);
                    }

                }

                jointPerson["jointLocations"] = jointLocations;
                jointPerson["personId"] = humanNumber;
                joints.append(jointPerson);

            }
            root["joints"] = joints;
            log << root << std::endl;
            log.close();

        }

    }
    catch (const std::exception& e)
    {
        op::error(e.what(), __LINE__, __FUNCTION__, __FILE__);

    }
}


int main(int argc, char *argv[])
{

    gflags::ParseCommandLineFlags(&argc, &argv, true);

    if(argc<3){
        std::cout<< "Program usage : openPoseTest.exe src_folder_path dest_folder_path --render_threshold=0.1" << std::endl;
        std::cout << " See all the available parameter options withe the `--help` flag " << std::endl;
        std::cout<<"Make sure source folders and destination folders exist" << std::endl;
    }

    QString src_folder_path = QString::fromStdString(argv[1]);
    QString dest_folder_path = QString::fromStdString(argv[2]);
    //bool bWriteImage = argv[3];


    QDir dir;
    if(!dir.exists(src_folder_path)){
        std::cout << "Image folder " << src_folder_path.toStdString()<< "does not exist" << std::endl;
        return 0;
    }
    if(!dir.exists(dest_folder_path)){
        std::cout << "Creating new director " << dest_folder_path.toStdString() << std::endl;
        dir.mkdir(dest_folder_path);

    }

    openPoseTutorialPose1(src_folder_path,dest_folder_path);
    return 0;
}

void openPoseTutorialPose1(QString folder_path,QString dest_folder_path)
{
    try
    {
        op::log("Starting OpenPose demo...", op::Priority::High);

        // ------------------------- INITIALIZATION -------------------------
        // Step 1 - Set logging level     // - 0 will output all the logging messages      // - 255 will output nothing
        op::check(0 <= FLAGS_logging_level && FLAGS_logging_level <= 255, "Wrong logging_level value.",
                  __LINE__, __FUNCTION__, __FILE__);
        op::ConfigureLog::setPriorityThreshold((op::Priority)FLAGS_logging_level);
        op::log("", op::Priority::Low, __LINE__, __FUNCTION__, __FILE__);
        // Step 2 - Read Google flags (user defined configuration)
        // outputSize
        const auto outputSize = op::flagsToPoint(FLAGS_output_resolution, "-1x-1");
        // netInputSize
        const auto netInputSize = op::flagsToPoint(FLAGS_net_resolution, "-1x368");
        // poseModel
        const auto poseModel = op::flagsToPoseModel(FLAGS_model_pose);
        // image directory


        // poseModel

        // Check no contradictory flags enabled
        if (FLAGS_alpha_pose < 0. || FLAGS_alpha_pose > 1.)
            op::error("Alpha value for blending must be in the range [0,1].", __LINE__, __FUNCTION__, __FILE__);
        if (FLAGS_scale_gap <= 0. && FLAGS_scale_number > 1)
            op::error("Incompatible flag configuration: scale_gap must be greater than 0 or scale_number = 1.",
                      __LINE__, __FUNCTION__, __FILE__);
        // Logging
        op::log("", op::Priority::Low, __LINE__, __FUNCTION__, __FILE__);
        // Step 3 - Initialize all required classes
        op::ScaleAndSizeExtractor scaleAndSizeExtractor(netInputSize, outputSize, FLAGS_scale_number, FLAGS_scale_gap);
        op::CvMatToOpInput cvMatToOpInput{poseModel};
        op::CvMatToOpOutput cvMatToOpOutput;
        op::PoseExtractorCaffe poseExtractorCaffe{poseModel, FLAGS_model_folder, FLAGS_num_gpu_start};
        op::PoseCpuRenderer poseRenderer{poseModel, (float)FLAGS_render_threshold, !FLAGS_disable_blending,
                                         (float)FLAGS_alpha_pose};
        op::OpOutputToCvMat opOutputToCvMat;
        op::FrameDisplayer frameDisplayer{"OpenPose Tutorial - Example 1", outputSize};
        // Step 4 - Initialize resources on desired thread (in this case single thread, i.e. we init resources here)
        poseExtractorCaffe.initializationOnThread();
        poseRenderer.initializationOnThread();




        // ------------------------- POSE ESTIMATION AND RENDERING -------------------------
        // Step 1 - Read and load image, error if empty (possibly wrong path)
        // Alternative: cv::imread(FLAGS_image_path, CV_LOAD_IMAGE_COLOR);
        //cv::Mat inputImage = op::loadImage(image_path, CV_LOAD_IMAGE_COLOR);



        std::cout << "Initialized keypoint names" << std::endl;


        QDir dir(folder_path);
        QStringList filters;
        filters << "*.jpg" << "*.png";
        dir.setNameFilters(filters);
        QFileInfoList list = dir.entryInfoList();
        std::cout << list.size() << " image files in folder" << std::endl;
        cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );// Create a window for display.

        const auto timerBegin = std::chrono::high_resolution_clock::now();

        for(int i=0;i<list.size();i++){
            QString image_path = list.at(i).filePath();

            QString imageName = list.at(i).fileName();
            //std::cout<<"Image path" << image_path.toStdString()<<std::endl;

            QStringList filename_list = image_path.split("/");
            QString image_name = filename_list.last();
            QString dest_path = dest_folder_path + "/" + image_name;

            cv::Mat inputImage = op::loadImage(image_path.toStdString(), CV_LOAD_IMAGE_COLOR);
            if(inputImage.empty())
                std::cout<<"Image empty" << std::endl;

            const op::Point<int> imageSize{inputImage.cols, inputImage.rows};
            // Step 2 - Get desired scale sizes
            std::vector<double> scaleInputToNetInputs;
            std::vector<op::Point<int>> netInputSizes;
             double scaleInputToOutput;
            op::Point<int> outputResolution;
            std::tie(scaleInputToNetInputs, netInputSizes, scaleInputToOutput, outputResolution)
                 = scaleAndSizeExtractor.extract(imageSize);

            // Step 3 - Format input image to OpenPose input and output formats
            const auto netInputArray = cvMatToOpInput.createArray(inputImage, scaleInputToNetInputs, netInputSizes);
            auto outputArray = cvMatToOpOutput.createArray(inputImage, scaleInputToOutput, outputResolution);

            // Step 4 - Estimate poseKeypoints
            poseExtractorCaffe.forwardPass(netInputArray, imageSize, scaleInputToNetInputs);
            const auto poseKeypoints = poseExtractorCaffe.getPoseKeypoints();

            const auto thresholdRectangle = 0.1f;
            const auto numberKeypoints = poseKeypoints.getSize(1);
            const auto areaKeypoints = numberKeypoints * poseKeypoints.getSize(2);


            // Step 5 - Render poseKeypoints
             poseRenderer.renderPose(outputArray, poseKeypoints, scaleInputToOutput);

            // Step 6 - OpenPose output format to cv::Mat
            auto outputImage = opOutputToCvMat.formatToCvMat(outputArray);         

            std::cout << i << ". " << image_path.toStdString()<<std::endl;

            std::thread t3(processKeyPoints,poseKeypoints,imageName.toStdString(),image_path.toStdString(), dest_folder_path.toStdString());
            t3.join();


            if(!poseKeypoints.empty()){

                if(bDrawRectangle){

                     for(int i=0; i<poseKeypoints.getSize(0);i++){
                        const auto personRectangle = op::getKeypointsRectangle(poseKeypoints, i, thresholdRectangle);
                        int x_0 = (int)personRectangle.x;
                         int y_0 = (int)personRectangle.y;
                        int width = (int)personRectangle.width;
                        int height = (int)personRectangle.height;

                        cv::Rect box(x_0,y_0,width,height);
                        cv::rectangle(outputImage, box, cv::Scalar(0, 255, 0),2);
                     }
                }
                bool bWriteImage = true;
                cv::imshow("Display window",outputImage);
                if(bWriteImage){
                 cv::imwrite(dest_path.toStdString(),outputImage);
                }
                cv::waitKey(10);
            }


            const auto now = std::chrono::high_resolution_clock::now();
            const auto totalTimeSec = (double)std::chrono::duration_cast<std::chrono::nanoseconds>(now-timerBegin).count()
                                 * 1e-9;
            const auto message = "OpenPose demo successfully finished. Total time: "
                            + std::to_string(totalTimeSec) + " seconds.";
            op::log(message, op::Priority::High);            

        }


    }
    catch (const std::exception& e)
    {
        op::error(e.what(), __LINE__, __FUNCTION__, __FILE__);

    }
}
