//ROS typedefs
#include "ros/ros.h"
#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/PointCloud2.h>

/* protected region user include files on begin */
#include <brics_3d/core/PointCloud3D.h>
#include <brics_3d/algorithm/filtering/MaskROIExtractor.h>
#include <brics_3d/algorithm/segmentation/RegionBasedSACSegmentation.h>
#include <brics_3d/util/PCLTypecaster.h>
#include <brics_3d/util/Timer.h>
#include <brics_3d/core/Logger.h>
/* protected region user include files end */

class DichotomySegmentor_config
{
public:

};

class DichotomySegmentor_data
{
// autogenerated: don't touch this class
public:
//input data
    	sensor_msgs::PointCloud2 in_inputPointCloud;
  
	
//output data
    	sensor_msgs::PointCloud2 out_inliers;
    	sensor_msgs::PointCloud2 out_outliers;
 

};

class DichotomySegmentor_impl
{
	/* protected region user member variables on begin */
	brics_3d::Timer timer;
	/* protected region user member variables end */

public:
    DichotomySegmentor_impl() 
    {
        /* protected region user constructor on begin */
		/* protected region user constructor end */
    }
    void configure(DichotomySegmentor_config config) 
    {
        /* protected region user configure on begin */
		brics_3d::Logger::setMinLoglevel(brics_3d::Logger::LOGDEBUG);
		/* protected region user configure end */
    }
    void update(DichotomySegmentor_data &data, DichotomySegmentor_config config)
    {
        /* protected region user update on begin */
    	/* Prepare data */
       	std::string referenceFrameId;
        referenceFrameId = data.in_inputPointCloud.header.frame_id;
        pcl::PointCloud<pcl::PointXYZ>::Ptr inputPointCloutPcl(new pcl::PointCloud<pcl::PointXYZ>);
        pcl::PointCloud<pcl::PointXYZ>::Ptr inlierPointCloudPcl(new pcl::PointCloud<pcl::PointXYZ>);
        pcl::PointCloud<pcl::PointXYZ>::Ptr outlierPointCloudPcl(new pcl::PointCloud<pcl::PointXYZ>);

        pcl::fromROSMsg(data.in_inputPointCloud, *inputPointCloutPcl);

        brics_3d::PointCloud3D inputPointCloud;
        brics_3d::PointCloud3D inlierPointCloud;
        brics_3d::PointCloud3D outlierPointCloud;

        brics_3d::PCLTypecaster caster;
        caster.convertToBRICS3DDataType(inputPointCloutPcl, &inputPointCloud);

		/* Segment the dominant plane */
		timer.reset();
		Eigen::VectorXd modelCoefficients;
		std::vector<int> inliers;
		brics_3d::RegionBasedSACSegmentation sacSegmenter;

		sacSegmenter.setPointCloud(&inputPointCloud);
		sacSegmenter.setDistanceThreshold(0.02);
		sacSegmenter.setMaxIterations(1000);
		sacSegmenter.setMethodType(sacSegmenter.SAC_RANSAC);
		sacSegmenter.setModelType(sacSegmenter.OBJMODEL_PLANE);
		sacSegmenter.setProbability(0.99);

    	ROS_INFO("Segmenting.");
		sacSegmenter.segment();
		sacSegmenter.getInliers(inliers);
		sacSegmenter.getModelCoefficients(modelCoefficients);

		cout << "Timer: Plane segmantation took " << timer.getElapsedTime() << "[ms]" << endl;
		cout << "Found Inliers: " << inliers.size() << "of " << inputPointCloud.getSize() << " total points." << endl;

		brics_3d::MaskROIExtractor indicesExtractor;
		indicesExtractor.setMask(&inliers);
		indicesExtractor.setUseInvertedMask(false); // inliers
		indicesExtractor.filter(&inputPointCloud, &inlierPointCloud);
		indicesExtractor.setUseInvertedMask(true); // outliers
		indicesExtractor.filter(&inputPointCloud, &outlierPointCloud);

		/* Prepare output */
    	caster.convertToPCLDataType(inlierPointCloudPcl, &inlierPointCloud);
    	pcl::toROSMsg(*inlierPointCloudPcl, data.out_inliers);
    	data.out_inliers.header.frame_id = referenceFrameId;

    	caster.convertToPCLDataType(outlierPointCloudPcl, &outlierPointCloud);
    	pcl::toROSMsg(*outlierPointCloudPcl, data.out_outliers);
    	data.out_outliers.header.frame_id = referenceFrameId;


		/* protected region user update end */
    }

    
    /* protected region user additional functions on begin */
	/* protected region user additional functions end */
    
};
