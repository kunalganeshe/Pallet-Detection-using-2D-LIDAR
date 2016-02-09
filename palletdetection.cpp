#include <QCoreApplication>
#include <LMS1xx.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <mlpack/methods/kmeans/kmeans.hpp>
#include <armadillo>

using namespace mlpack::kmeans;
using namespace arma;

#define host "192.168.16.210"
#define DEG2RAD M_PI/180.0
#define RAD2DEG 180.0/M_PI

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    static int index = 0;
    ///Laser Data
    LMS1xx LaserSensor;
    scanCfg cfg;
    scanData data;
    scanDataCfg dataCfg;
    status_t status;

    std::ofstream file;

    double start_angle=0;
    double stop_angle=0;
    double resolution=0;
    double frequency=0;

    ///Kmeans realated variables
    KMeans<> K;
    mat dataset;
    size_t cluster;
    Col<size_t> assignments;
    mat centroid;


    ///Connect to the Lasersensor
    LaserSensor.connect(host);

    if(LaserSensor.isConnected())
    {
        std::cout << "\nConnected !!!\n";

        LaserSensor.login();

        ///Get Laser Configurations
        cfg = LaserSensor.getScanCfg();
        //cfg.angleResolution = 0.25*10000.0;
        //cfg.scaningFrequency = 25*100;

        //LaserSensor.setScanCfg(cfg);
        //LaserSensor.saveConfig();
       // sleep(3);
        cfg = LaserSensor.getScanCfg();
        start_angle = cfg.startAngle/10000.0; //* DEG2RAD - M_PI/2;
        stop_angle = cfg.stopAngle/10000.0; //* DEG2RAD - M_PI/2;
        resolution = cfg.angleResolution/10000.0;
        frequency = cfg.scaningFrequency/100;

        std::cout << "Start Angle: " << start_angle;
        std::cout << "\tStop Angle: " << stop_angle;
        std::cout << "\tResolution: " << resolution;
        std::cout << "\tFrequency: " << frequency;
        std::cout << std::endl;

        dataCfg.outputChannel = 1;
        dataCfg.remission = true;
        dataCfg.resolution = 1;
        dataCfg.encoder = 0;
        dataCfg.position = false;
        dataCfg.deviceName = false;
        dataCfg.outputInterval = 1;

        LaserSensor.setScanDataCfg(dataCfg); ///Set Data Configuration of the laser data

        LaserSensor.startMeas();    ///Start Measurement

        do
        {
            status = LaserSensor.queryStatus();
            usleep(200);
        }
        while(status != ready_for_measurement);
        {
            LaserSensor.startDevice();
            LaserSensor.scanContinous(1);

            while(LaserSensor.isConnected())
            {
                LaserSensor.getData(data);  ///Get the Laser Data

                //                u_int16_t range[data.dist_len1];
                //                u_int16_t intensity[data.rssi_len1];
                int range[data.dist_len1];
                int intensity[data.rssi_len1];

                for(int i=0; i<data.dist_len1;i++)
                    range[i] = data.dist1[i];

                for(int i=0; i<data.rssi_len1;i++)
                    intensity[i] = data.rssi1[i];

                if (index == 0)
                {
                    index++;
                    std::cout << std::endl << "Data len = " << data.dist_len1 << std::endl;
                    std::cout << "Intensity len = " << data.rssi_len1 << std::endl;

                    ///distance assumed to be in mm
                    ///Start angle is -45 end is 225
                    float angle_scan = -45.0;
                    float x[1081], y[1081];   ///The resolution is 0.5 degress so 541 values
                    int index_range = 0;
                    double slope;
                    cluster = 2;
                    //centroid.zeros();
                    dataset.resize(2,1081);
                    dataset.zeros();

                    file.open("LaserData.txt");

                    while(1)
                    {
                        x[index_range] = range[index_range]*cos(angle_scan*DEG2RAD)/1000.0;
                        y[index_range] = range[index_range] * sin(angle_scan*DEG2RAD)/1000.0;
                        //std::cout << "range: " << range[index_range] << " angle: " << angle_scan;
                        //std::cout << " x: " << x[index_range] << " y : " << y[index_range] << std::endl;
                        angle_scan += 0.25;

                        //if(intensity[index_range] >=850)
                        {
                            file << x[index_range] << "," << y[index_range] << "," << intensity[index_range] << std::endl;
                        }

                        if (angle_scan > 225.0)
                        {
                            break;
                        }
                        index_range++;
                        usleep(100);
                    }
                    int index_tmp = 0;
                    for(int i=0; i<1081;i++)
                    {
                        if (intensity[i] >= 900)
                        {
                            dataset(0,index_tmp) = x[i];
                            dataset(1,index_tmp) = y[i];

                            std::cout << "\n" << dataset[0,index_tmp] << "\t" << dataset[1,index_tmp];
                            index_tmp++;
                        }
                    }
                    std::cout << "\nKMeans Calculations!!!" << std::endl;
                    dataset.resize(2,index_tmp);

                    ///Actual KMeans CLustering
                    K.Cluster((arma::mat) dataset,2,assignments,centroid);

/*************************************************************************************************************************
                    static double sum_x[2];
                    static double sum_y[2];
                    int number_dist1=0;
                    int number_dist2=0;

                    for(int i=0; i < assignments.size(); i++) {
                        switch(assignments[i])
                        {
                            case 0:
                                sum_x[0]+=dataset(0,i);
                                sum_y[0]+=dataset(1,i);
                                number_dist1++;
                                break;
                            case 1:
                                sum_x[1]+=dataset(0,i);
                                sum_y[1]+=dataset(1,i);
                                number_dist2++;
                                break;
                        };

                        std::cout << "\n" << assignments[i];
                    }

                    double center1_x, center1_y;
                    double center2_x, center2_y;

                    center1_x = sum_x[0]/number_dist1;
                    center1_y = sum_y[0]/number_dist1;
                    center2_x = sum_x[1]/number_dist2;
                    center2_y = sum_y[1]/number_dist2;

                    std::cout<<center1_x<<"," << center1_y<<"   " << center2_x<<","<<center2_y<<endl;

**************************************************************************************************************************/

//std::cout << "\n" << centroid(0,0) << "\t" << centroid(1,0) << "\t"<< centroid(0,1) << "\t"<< centroid(1,1)<<"\n";

                    slope = (centroid(1,1) - centroid(1,0)) / (centroid(0,1) - centroid(0,0));
                    slope = (atan(slope))*RAD2DEG;

                    std::cout << "\nclusters= " << cluster << std::endl;
                    std::cout << "\nOrientation= " << slope << std::endl;
                }
                usleep(200);
            }
            std::cout << "\n Sensor Disconnected \n";

            ///Disconnect the Laser
            LaserSensor.scanContinous(0);
            LaserSensor.stopMeas();
            LaserSensor.disconnect();
            file.close();
        }
    }
    else
    {
        std::cout <<"\nSensor Not Connected !!!\n";
    }

    return a.exec();
}
