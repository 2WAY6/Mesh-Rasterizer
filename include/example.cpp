#include "iostream"
#include "iostream"
#include "fstream"
#include "string"
#include "cstdlib"
#include "netcdfcpp.h"
#include "gdal_priv.h"
#include "cpl_conv.h"
#include "limits.h"
#include "stdlib.h"
using namespace std;
typedef std::string String;
 
/* declare constant integer
* variables for dimensions (nrows,ncols)
* as well as error code
*/
 
static const int NROWS = 700;
static const int NCOLS = 700;
static const int NC_ERR = 12;
 
int main() {
 
  /* register all known GDAL drivers.
   * attempt to suppress GDAL warnings.
  */
 
  GDALAllRegister();
  CPLPushErrorHandler(CPLQuietErrorHandler);
 
  /* declare pointer variables that will hold memory
   * addresses (or "point to") for two GDALDataset
   * objects, one for the grib file and one for the
   * output Geotiff.
   */
 
  GDALDataset *gribDataset, *geotiffDataset;
  GDALDriver *driverGeotiff;
  GDALRasterBand *geotiffBand; // also declare pointers for Geotiff
                               // and raster band object(s)
 
  /* declare array variables for reflectivity (dBZ) as well
   * as 6-element geotransform. Create variables for numbers
   * of rows, columns, and NoData fill value.
   */
 
  double dbz[NROWS][NCOLS];
  int nrows,ncols,noData;
  double transform[6];
 
  /* declare variables for names of netcdf
   * and corresponding GRIB files,
   * create Netcdf file reader object
   */
 
  const char* grib_filename = "nmbprs_d01.0300.grb";
  String netcdf_filename = "nmbprs_d01.0300.nc";
  NcFile dataFile(netcdf_filename.c_str() , NcFile::ReadOnly);
 
  if(!dataFile.is_valid()) {
    return NC_ERR;
  }
 
  /* open up NETCDF file, make sure it is successfully opened,
   * exit and return error code if failure
   */
 
  /* read max. composite reflectivity (i.e. dBZ) into 2D
   * array from NETCDF. First we create pointer variable to point
   * to memory address that holds an NcVar object. Then we call
   * the get() method on that pointer variable, passing in the
   * memory location of the first element dbz[][] array as well
   * as the integer dimensions.
   */
 
  NcVar *refcData = dataFile.get_var("REFC_GDS3_EATM");
  refcData->get(&dbz[0][0],NROWS,NCOLS);
 
  /* Read Geospatial attributes from GRIB file.
   * First call GDALOpen() method, passing in name of
   * GRIB file. Then cast this object to a pointer
   * variable that will point to, or hold the address
   * of a GDALDataset object.
   */
 
  gribDataset = (GDALDataset*) GDALOpen(grib_filename, GA_ReadOnly);
 
  /* pass in 6-element empty geotransform transform[] array
   * to pointer object gribDataset's GetGeoTransform()
   * method to fill in values for geotransform in transform[]
   * array. Then also getll GetYSize(), GetXSize(), and
   * GetNoDataValue() to get the X/Y dimensions and
   * NoData (Fill) value for GRIB file.
   */
 
  gribDataset->GetGeoTransform(transform);
  nrows = gribDataset->GetRasterBand(1)->GetYSize();
  ncols = gribDataset->GetRasterBand(1)->GetXSize();
  noData = gribDataset->GetRasterBand(1)->GetNoDataValue();
 
  /* Create Geotiff filename string that will hold
   * array dBZ variable as 2D array
   */
 
  String extension(".tif"), tiffname;
  tiffname = (String)grib_filename+extension;
 
  /* create GDAL driver object whose Create() method will be used
   * to create Geotiff writer object. Get geotransform using
   * 6-element array from above. Read projection from GRIB
   * file and use SetProjection() method to set the projection
   * of the Geotiff object.
   */
 
  driverGeotiff = GetGDALDriverManager()->GetDriverByName("GTiff");
  geotiffDataset = driverGeotiff->Create(tiffname.c_str(),ncols,nrows,1,GDT_Float32,NULL);
  geotiffDataset->SetGeoTransform(transform);
  geotiffDataset->SetProjection(gribDataset->GetProjectionRef());
 
  /* allocate memory block (essentially sequential array of
   * memory cells) to hold a row's worth of array data (from
   * within the 2D array). To this end, we create a
   * pointer variable that holds the memory address,
   * or "points to", a float. This float is actually
   * the first element of an array that is pointed
   * to by *rowBuff.
   */
 
  float *rowBuff = (float*) CPLMalloc(sizeof(float)*ncols);
 
  /* iterate through all pixels in 2D dBZ
   * reflectivity array and assign values into rowBuff
   * array. Then write each row into Geotiff object.
   */
 
  for(int row=0; row<nrows; row++) {
    for(int col=0; col<ncols; col++) {
        rowBuff[col] = (float)dbz[row][col];
    }
    geotiffDataset->GetRasterBand(1)->RasterIO(GF_Write,0,row,ncols,1,rowBuff,ncols,1,GDT_Float32,0,0);
  }
 
  /* close out grib datsaet, new geotiff dataset objs.
   * close out GDAL driver manager.
   */
 
  GDALClose(gribDataset);
  GDALClose(geotiffDataset) ;
  GDALDestroyDriverManager();
  return 0;
 
}