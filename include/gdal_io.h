#include <iostream>
#include <string>
#include <vector>
#include "gdal_priv.h"
#include "raster.h"

void export_to_geotiff (Raster& raster) {
    GDALAllRegister();

    const char *pszFormat = "GTiff";
    GDALDriver *poDriver;
    char **papszMetadata;
    poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
    if( poDriver == NULL ){
        std::cout << ":(" << std::endl;
        exit( 1 );
    }
    papszMetadata = poDriver->GetMetadata();
    if( CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATE, FALSE ) )
        printf( "Driver %s supports Create() method.\n", pszFormat );
    if( CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATECOPY, FALSE ) )
        printf( "Driver %s supports CreateCopy() method.\n", pszFormat );

    std::cout << "xMin " << raster.xMin << std::endl;
    std::cout << "xMax " << raster.xMax << std::endl;
    std::cout << "nCols " << raster.nCols << std::endl;
    std::cout << "yMin " << raster.yMin << std::endl;
    std::cout << "yMax " << raster.yMax << std::endl;
    std::cout << "nRows " << raster.nRows << std::endl;

    GDALDataset *poDstDS;
    char **papszOptions = NULL;
    // poDstDS = poDriver->Create( "Test.tif", 512, 512, 1, GDT_Byte,
    //                             papszOptions ); 
    poDstDS = poDriver->Create( "Test.tif", raster.nCols, raster.nRows, 1, GDT_Float32,
                                papszOptions ); 

    double adfGeoTransform[6] = { double(raster.xMin), 1, 0, double(raster.yMin), 0, 1 };
    // OGRSpatialReference oSRS;
    // char *pszSRS_WKT = NULL;
    GDALRasterBand *poBand;
    GByte abyRaster[1];

    std::cout << "A" << std::endl;
    float p[raster.nRows * raster.nCols];
    // float p[raster.nRows][raster.nCols];
    std::cout << "B" << std::endl;

    int i = 0;
    double zMin = 10000000000;
    double zMax = 0;
    for (int rowi = 0; rowi < raster.nRows; rowi++) {
        for (int coli = 0; coli < raster.nCols; coli++) {
            // p[rowi][coli] = raster.matrix[rowi][coli];
            p[i] = raster.matrix[rowi][coli];

            if (p[i] > zMax)
                zMax = raster.matrix[rowi][coli];
            else if (p[i] < zMin)
                zMin = raster.matrix[rowi][coli];

            i++;
        }
    }
    std::cout << "zMin: " << zMin << std::endl;
    std::cout << "zMax: " << zMax << std::endl;
    std::cout << "C" << std::endl;
    // GByte abyRaster[512*512];
    poDstDS->SetGeoTransform( adfGeoTransform );
    // oSRS.SetUTM( 11, TRUE );
    // oSRS.SetWellKnownGeogCS( "NAD27" );
    // oSRS.exportToWkt( &pszSRS_WKT );
    // poDstDS->SetProjection( pszSRS_WKT );
    // CPLFree( pszSRS_WKT );
    poBand = poDstDS->GetRasterBand(1);
    // poBand->RasterIO( GF_Write, 0, 0, 512, 512,
    //                 abyRaster, 512, 512, GDT_Byte, 0, 0 );
    std::cout << "D" << std::endl;
    poBand->RasterIO( GF_Write, 0, 0, raster.nCols, raster.nRows,
                    p, raster.nCols, raster.nRows, GDT_Float32, 0, 0 );
    double dfMin = 0;
    double dfMax = 0;
    double dfMean = 0;
    double dfStdDev = 0;                
    poBand->ComputeStatistics(1, &dfMin, &dfMax, &dfMean, &dfStdDev, NULL, nullptr);
    std::cout << dfMin << " | " << dfMax << " | " << dfMean << std::endl;
    dfMin = raster.zMin;
    dfMax = raster.zMax;
    poBand->SetStatistics(dfMin, dfMax, dfMean, dfStdDev);
    /* Once we're done, close properly the dataset */
    GDALClose( (GDALDatasetH) poDstDS );

    std::cout << "Finished" << std::endl;

}