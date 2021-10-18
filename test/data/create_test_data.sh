mkdir las
cd las
wget https://github.com/PDAL/data/raw/a3d2a351ca1002c7ea4daa96b5c5fcb0fafeaa6f/autzen/autzen-classified.copc.laz
las2las -i autzen-classified.copc.laz -remove_all_vlrs -remove_all_evlrs -start_at_point 0 -stop_at_point 20 -o first_20_pts.las
las2las -i autzen-classified.copc.laz -remove_all_vlrs -remove_all_evlrs -start_at_point 20 -stop_at_point 32 -o next_12_pts.las
las2las -i autzen-classified.copc.laz -remove_all_vlrs -remove_all_evlrs -start_at_point 32 -stop_at_point 92 -o last_60_pts.las
laszip first_20_pts.las
laszip next_12_pts.las
laszip last_60_pts.las