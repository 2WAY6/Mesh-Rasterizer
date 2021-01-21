#include <iostream>
#include "gdal_priv.h"

int main () {
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

    GDALDataset *poDstDS;
    char **papszOptions = NULL;
    poDstDS = poDriver->Create( "Test.tif", 512, 512, 1, GDT_Byte,
                                papszOptions ); 

    double adfGeoTransform[6] = { 444720, 30, 0, 3751320, 0, -30 };
    OGRSpatialReference oSRS;
    char *pszSRS_WKT = NULL;
    GDALRasterBand *poBand;
    GByte abyRaster[512*512];
    poDstDS->SetGeoTransform( adfGeoTransform );
    oSRS.SetUTM( 11, TRUE );
    oSRS.SetWellKnownGeogCS( "NAD27" );
    oSRS.exportToWkt( &pszSRS_WKT );
    poDstDS->SetProjection( pszSRS_WKT );
    CPLFree( pszSRS_WKT );
    poBand = poDstDS->GetRasterBand(1);
    poBand->RasterIO( GF_Write, 0, 0, 512, 512,
                    abyRaster, 512, 512, GDT_Byte, 0, 0 );
    /* Once we're done, close properly the dataset */
    GDALClose( (GDALDatasetH) poDstDS );

    std::cout << "Finished" << std::endl;

}