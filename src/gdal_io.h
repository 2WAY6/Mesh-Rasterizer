#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "gdal_priv.h"
#include "raster.h"

void export_to_geotiff (Raster& raster) {
    std::cout << "\nExporting raster as GeoTIFF..." << std::endl;

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
    // if( CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATE, FALSE ) )
    //     printf( "Driver %s supports Create() method.\n", pszFormat );
    // if( CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATECOPY, FALSE ) )
    //     printf( "Driver %s supports CreateCopy() method.\n", pszFormat );

    GDALDataset *poDstDS;
    char **papszOptions = NULL;
    poDstDS = poDriver->Create("Test.tif", raster.nCols, raster.nRows, 1, 
                               GDT_Float32, papszOptions); 

    double adfGeoTransform[6] = {double(raster.xMin), 1, 0, 
                                 double(raster.yMax), 0, -1};
    GDALRasterBand *poBand;
    poDstDS->SetGeoTransform(adfGeoTransform);
    // oSRS.SetUTM( 11, TRUE );
    // oSRS.SetWellKnownGeogCS( "NAD27" );
    // oSRS.exportToWkt( &pszSRS_WKT );
    // poDstDS->SetProjection( pszSRS_WKT );
    // CPLFree( pszSRS_WKT );
    poBand = poDstDS->GetRasterBand(1);

    float *rowBuff = (float*) CPLMalloc(sizeof(float)*raster.nCols);
    
    float zMax = 10000000;
    float zMin = 0;
    for (int rowi = 0; rowi < raster.nRows; rowi++) {
        for (int coli = 0; coli < raster.nCols; coli++) {
            rowBuff[coli] = (float)raster.matrix[rowi][coli];
           
            if (rowBuff[coli] > zMax)
                zMax = raster.matrix[rowi][coli];
            else if (rowBuff[coli] < zMin)
                zMin = raster.matrix[rowi][coli];
        }
        poBand->RasterIO(GF_Write, 0, rowi, raster.nCols, 1, rowBuff, 
                         raster.nCols, 1, GDT_Float32, 0, 0);
    }

    float dfMin = raster.zMin;
    float dfMax = raster.zMax;
    poBand->SetStatistics(dfMin, dfMax, 0.0, 0.0);

    GDALClose( (GDALDatasetH) poDstDS );

    std::cout << "- Finished writing raster." << std::endl;
}

