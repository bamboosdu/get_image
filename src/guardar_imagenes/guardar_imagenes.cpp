#include <ros/ros.h>
#include <cv_bridge/cv_bridge.h>  // OpenCV
#include <sensor_msgs/Image.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
 
#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#define APPROXIMATE
#ifdef EXACT
#include <message_filters/sync_policies/exact_time.h>
#endif
#ifdef APPROXIMATE
#include <message_filters/sync_policies/approximate_time.h>
#endif
 
  
  
using namespace std;
//using namespace sensor_msgs;
using namespace message_filters;
 
 
// Contador para la numeración de los archivos.
// Counter for filenames.
unsigned int cnt = 1;
 
 
 
// Handler / callback
void callback( const sensor_msgs::ImageConstPtr& msg_rgb , const sensor_msgs::ImageConstPtr& msg_depth )
{
    ROS_INFO_STREAM(" callback\n");
      cv_bridge::CvImagePtr img_ptr_rgb;
        cv_bridge::CvImagePtr img_ptr_depth;
    try{
        img_ptr_depth = cv_bridge::toCvCopy(*msg_depth, sensor_msgs::image_encodings::TYPE_16UC1);//TYPE_16UC1 TYPE_32FC1
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("cv_bridge exception:  %s", e.what());
        return;
    }
    try{
        img_ptr_rgb = cv_bridge::toCvCopy(*msg_rgb, sensor_msgs::image_encodings::BGR8);//TYPE_8UC3  BGR8
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("cv_bridge exception:  %s", e.what());
        return;
    }
ROS_INFO_STREAM(" convert ok!\n");
    cv::Mat& mat_depth = img_ptr_depth->image;
        cv::Mat& mat_rgb = img_ptr_rgb->image;
         
        char file_rgb[100];//"/home/t3/zq/rgbd/rgb"
        char file_depth[100];//"/home/t3/zq/rgbd/depth"
         
        sprintf(file_rgb , "/home/t3/zq/rgbd/rgb/%07d_rgb.png", cnt );//_rgb
        sprintf(file_depth, "/home/t3/zq/rgbd/depth/%07d_depth.png", cnt );//_depth
         
        vector<int> png_parameters;
        png_parameters.push_back( CV_IMWRITE_PNG_COMPRESSION );
        /* We save with no compression for faster processing.
         * Guardamos PNG sin compresión para menor retardo. */
        png_parameters.push_back( 9 );
         
        cv::imwrite( file_rgb , mat_rgb, png_parameters );
        cv::imwrite( file_depth, mat_depth, png_parameters );
        string rgb_path=file_rgb;
        string depth_path=file_depth;
        ROS_INFO_STREAM("RGB data saved to"+rgb_path+"\n");
        ROS_INFO_STREAM("DEPTH data saved to"+depth_path+"\n");
        ROS_INFO_STREAM(cnt << "\n");
        ROS_INFO_STREAM("Ok!"
                        "Images saved\n");
         
        cnt++;
         
}
 
 
 
 
 
int main(int argc, char** argv)
{
    // Initialize the ROS system and become a node.
  ros::init(argc, argv, "guardar_imagenes");
  ros::NodeHandle nh;
     
     
    message_filters::Subscriber<sensor_msgs::Image> subscriber_depth( nh , "/camera/depth/image_raw" , 1 );
    message_filters::Subscriber<sensor_msgs::Image> subscriber_rgb( nh , "/camera/rgb/image_raw" , 1 );
 
     
#ifdef EXACT
    typedef sync_policies::ExactTime<sensor_msgs::Image, sensor_msgs::Image> MySyncPolicy;
#endif
#ifdef APPROXIMATE
    typedef sync_policies::ApproximateTime<sensor_msgs::Image, sensor_msgs::Image> MySyncPolicy;
#endif
     
     
  // ExactTime or ApproximateTime take a queue size as its constructor argument, hence MySyncPolicy(10)
  Synchronizer<MySyncPolicy> sync(MySyncPolicy(10), subscriber_rgb, subscriber_depth );
  sync.registerCallback(boost::bind(&callback, _1, _2));
 
     
    while(ros::ok())
    {
        char c;
         
        ROS_INFO_STREAM("\nIngrese 'a' para guardar un par de imágenes o 'b' para guardar 300 imágenes\n"
                        "Enter 'a' to save a pair of images or 'b' to automatically save 300 images\n");
       // cin.get(c);
       // cin.ignore();
       // c = tolower(c);
       // ROS_INFO_STREAM("You entered " << c << "\n");
         
        //if( c == 'a' )
        {
                /* Le damos el control a la función callback cuando haya imágenes.
                * We give control to the callback function.*/
               ROS_INFO_STREAM("I am running! \n");       
                   ros::spin();
                ROS_INFO_STREAM("I am OVER! \n");  
        }
         
        // else if( c == 'b' )
        // {
        //     unsigned int cnt_init = cnt;
        //     while( cnt - cnt_init < 300 )
        //     {
        //         ros::spinOnce();
        //     }
        // }
         
        // else break;
 
    }
    ROS_INFO_STREAM("Cerrando nodo\nClosing node\n");
 
  return 0;
}