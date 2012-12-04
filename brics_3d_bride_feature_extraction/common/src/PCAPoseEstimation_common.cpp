//ROS typedefs
#include "ros/ros.h"
#include <geometry_msgs/PoseStamped.h>
#include <sensor_msgs/PointCloud2.h>

/* protected region user include files on begin */
#include <brics_3d/core/HomogeneousMatrix44.h>
#include <brics_3d/algorithm/featureExtraction/BoundingBox3DExtractor.h>
#include <brics_3d/util/PCLTypecaster.h>

#include <brics_3d_ros/SceneGraphTypeCasts.h>
/* protected region user include files end */

class PCAPoseEstimation_config
{
public:

};

class PCAPoseEstimation_data
{
// autogenerated: don't touch this class
public:
//input data
    	sensor_msgs::PointCloud2 in_inputPointCloud;
  
	
//output data
    	geometry_msgs::PoseStamped out_estimatedPose;
 

};

class PCAPoseEstimation_impl
{
	/* protected region user member variables on begin */
	brics_3d::BoundingBox3DExtractor* boundingBoxExtractor;
	/* protected region user member variables end */

public:
    PCAPoseEstimation_impl() 
    {
        /* protected region user constructor on begin */
		/* protected region user constructor end */
    }
    void configure(PCAPoseEstimation_config config) 
    {
        /* protected region user configure on begin */
    	boundingBoxExtractor = new brics_3d::BoundingBox3DExtractor();
		/* protected region user configure end */
    }
    void update(PCAPoseEstimation_data &data, PCAPoseEstimation_config config)
    {
        /* protected region user update on begin */
    	/* prepare input */
    	std::string referenceFrameId;
    	referenceFrameId = data.in_inputPointCloud.header.frame_id;
    	pcl::PointCloud<pcl::PointXYZ>::Ptr inputPointCloutPcl(new pcl::PointCloud<pcl::PointXYZ>);
    	pcl::fromROSMsg(data.in_inputPointCloud, *inputPointCloutPcl);
    	brics_3d::PointCloud3D inputPointCloud;
    	brics_3d::PCLTypecaster caster;
    	caster.convertToBRICS3DDataType(inputPointCloutPcl, &inputPointCloud);

    	/* do computation (PCA based pose estimation) */
    	ROS_INFO_STREAM("Estimating pose.");
    	brics_3d::IHomogeneousMatrix44::IHomogeneousMatrix44Ptr estimatedTransform(new brics_3d::HomogeneousMatrix44());
    	brics_3d::Vector3D resultDimensions; // here we do not use this information
    	boundingBoxExtractor->computeOrientedBoundingBox(&inputPointCloud, estimatedTransform.get(), resultDimensions);

    	/* prepare output */
    	tf::Transform tfTransform;
    	brics_3d::rsg::SceneGraphTypeCasts::convertHomogeniousMatrixToTfTransform(estimatedTransform, tfTransform);

    	data.out_estimatedPose.header = data.in_inputPointCloud.header;

    	data.out_estimatedPose.pose.position.x = tfTransform.getOrigin().getX();
    	data.out_estimatedPose.pose.position.y = tfTransform.getOrigin().getY();
    	data.out_estimatedPose.pose.position.z = tfTransform.getOrigin().getZ();

    	data.out_estimatedPose.pose.orientation.x = tfTransform.getRotation().getX();
    	data.out_estimatedPose.pose.orientation.y = tfTransform.getRotation().getY();
    	data.out_estimatedPose.pose.orientation.z = tfTransform.getRotation().getZ();
    	data.out_estimatedPose.pose.orientation.w = tfTransform.getRotation().getW();

		/* protected region user update end */
    }

    
    /* protected region user additional functions on begin */
	/* protected region user additional functions end */
    
};