/*
  FILE....: HRA_112_112.c

  Static arrays for LDPC codec HRA_112_112, generated by ldpc_gen_c_h_file.m.
*/

#include <stdint.h>
#include "HRA_112_112.h"

namespace FreeDV
{

const uint16_t HRA_112_112_H_rows[] = {
22, 18, 15, 63, 16, 13, 1, 2, 29, 25, 28, 4, 36, 10, 38, 7, 60, 23, 11, 38, 28, 1, 12, 31, 57, 45, 57, 30, 23, 59, 67, 14, 16, 4, 14, 62, 15, 50, 7, 70, 64, 6, 42, 48, 9, 31, 19, 40, 49, 2, 25, 3, 41, 49, 36, 9, 29, 39, 31, 5, 17, 1, 29, 25, 11, 21, 18, 2, 8, 22, 39, 15, 8, 22, 13, 3, 19, 4, 21, 62, 34, 43, 6, 24, 17, 60, 8, 74, 6, 44, 60, 10, 33, 12, 26, 24, 45, 81, 69, 80, 41, 28, 23, 5, 10, 20, 52, 18, 13, 86, 3, 7, 59, 21, 65, 72, 34, 37, 26, 55, 47, 48, 34, 5, 44, 47, 68, 96, 82, 111, 61, 74, 30, 17, 55, 98, 81, 66, 89, 35, 74, 82, 91, 51, 55, 51, 30, 89, 61, 75, 40, 71, 73, 11, 56, 54, 19, 47, 94, 69, 64, 20, 64, 12, 54, 77, 42, 88, 36, 52, 90, 63, 70, 27, 32, 73, 91, 32, 56, 46, 9, 78, 51, 68, 88, 67, 20, 43, 40, 14, 66, 86, 39, 97, 38, 27, 50, 84, 54, 92, 61, 46, 67, 24, 58, 35, 58, 37, 98, 85, 73, 84, 48, 35, 57, 16, 26, 37, 65, 32, 72, 95, 107, 33, 77, 33, 85, 105, 106, 75, 56, 71, 79, 59, 52, 105, 79, 90, 93, 100, 88, 112, 86, 80, 65, 42, 106, 100, 93, 94, 99, 97, 93, 101, 111, 99, 83, 53, 85, 95, 108, 107, 41, 109, 84, 78, 104, 101, 69, 110, 98, 103, 80, 83, 77, 71, 76, 78, 87, 102, 104, 95, 96, 83, 87, 50, 110, 103, 112, 45, 58, 70, 94, 91, 89, 81, 101, 82, 63, 72, 100, 97, 76, 112, 53, 105, 49, 75, 109, 102, 66, 111, 68, 87, 92, 79, 96, 43, 90, 44, 110, 99, 102, 92, 103, 106, 62, 53, 27, 46, 108, 104, 107, 108, 109, 76
};

const uint16_t HRA_112_112_H_cols[] = {
7, 8, 52, 12, 12, 42, 16, 69, 45, 14, 19, 23, 6, 32, 3, 5, 22, 2, 45, 50, 2, 1, 18, 84, 10, 7, 62, 11, 9, 21, 24, 63, 2, 5, 28, 13, 6, 15, 58, 39, 39, 22, 76, 13, 26, 68, 9, 10, 49, 38, 32, 11, 34, 44, 8, 7, 25, 67, 1, 17, 19, 36, 4, 41, 3, 26, 31, 15, 45, 40, 8, 4, 41, 20, 6, 53, 1, 42, 9, 20, 25, 17, 33, 41, 3, 19, 55, 17, 27, 14, 31, 88, 15, 26, 36, 16, 28, 24, 27, 16, 30, 56, 48, 43, 4, 5, 38, 37, 40, 46, 18, 18, 22, 50, 76, 34, 60, 83, 39, 73, 56, 92, 42, 52, 75, 35, 37, 33, 61, 67, 47, 75, 66, 70, 29, 92, 51, 95, 84, 21, 57, 28, 46, 66, 93, 11, 94, 55, 96, 20, 71, 48, 53, 43, 82, 90, 66, 90, 14, 44, 54, 62, 34, 58, 81, 53, 23, 43, 27, 93, 10, 86, 37, 80, 60, 49, 21, 79, 74, 72, 48, 61, 40, 76, 64, 29, 38, 79, 51, 54, 13, 49, 72, 30, 50, 86, 35, 80, 61, 56, 36, 59, 65, 91, 25, 47, 58, 59, 78, 47, 32, 24, 44, 86, 64, 57, 12, 23, 109, 107, 85, 63, 31, 65, 62, 68, 111, 78, 104, 89, 112, 87, 69, 105, 65, 94, 109, 78, 72, 104, 85, 108, 77, 106, 79, 74, 103, 96, 64, 105, 105, 102, 63, 35, 59, 108, 112, 81, 102, 57, 106, 83, 81, 77, 101, 55, 94, 96, 97, 106, 46, 101, 83, 85, 71, 107, 104, 87, 33, 67, 103, 95, 30, 91, 89, 103, 75, 51, 107, 87, 91, 89, 99, 68, 52, 109, 99, 88, 84, 112, 54, 70, 92, 100, 98, 74, 60, 100, 98, 110, 90, 73, 71, 95, 70, 100, 29, 69, 110, 93, 82, 97, 98, 77, 73, 99, 101, 108, 82, 102, 111, 110, 111, 97, 88, 80
};
const float HRA_112_112_input[] = {
-3.7496794787890972, 14.372112019392226, -7.5640452729302359, 6.9426063455159657, 5.3103644888713299, -6.9203550501252273, 8.4296575778653775, 13.495087143587781, 18.111520666852243, -9.9125748623510912, 10.601298534930972, -10.468591112149715, -9.0757329437720475, -14.471433733514324, 5.2048820572852641, -11.353785810284556, -9.4511008284496416, -9.5255219979484025, -2.0499245561876696, -9.8739646459388748, 22.03442141444015, -9.9745566449839878, -8.4276711655946226, -4.9811962116476307, -13.018434575859896, -5.3358535334627293, -5.6704294937789648, 14.243964608060018, -11.417925510314507, 9.1332657371467878, -14.380214782394296, 14.090409878618974, 6.5602278279998272, 15.53025696352436, -9.1752771765906616, -11.384503450560766, 12.240329442222599, -12.640059450058276, -11.824715154614376, -13.487656131954735, 15.38073452845444, -13.816294924566529, 6.3461114450644454, -2.5192445130977559, -11.916088712873863, 5.4360722876642518, 0.038031547223147381, -12.367220238860654, -2.747864039796549, -14.920508782249289, 16.487336720060863, -13.290002442259247, 19.142698450560925, -0.39443060583296108, 11.723442316413736, -3.6131702833965047, -4.6196487103817017, -11.794290650694531, -14.342351103186955, 2.8079943208330334, -15.290175151123936, 9.0801740558512414, 10.184385069676226, 8.400722260237572, 9.3504690108712936, -14.223531676384166, 11.752768386971752, 11.36995822251677, -15.285021241405444, -13.070613695054403, -11.869191325617697, 4.3191750845563401, 2.0836933404582791, -16.363829786416495, -5.7778094839806595, 11.06389861779129, 13.285433846434705, 9.2552396418849021, -11.065999403824057, -10.167040394420443, -7.0107225044503565, 2.3886881673282474, 5.0014484787306932, -9.2464083853314278, -12.043309174487364, -11.638411967211738, -16.302815497922911, 13.347129717938067, -4.1390259986125226, 0.7947480277507295, 11.538620744796759, -7.4410706619926028, 14.572449028311253, 12.392747919231169, -3.3027890746379289, -9.8431096813736687, 11.582657487369399, -7.85736442083219, -7.3780721969188443, -7.4006260265172212, -8.3937994980934327, -6.6804071011469555, 19.656301355404196, 1.1084340389939762, 3.6028635453146465, -4.5409495140900562, 7.3831459854578982, -5.5905999874445662, -13.852328482738232, 8.9999210644983041, 8.4742375282492315, 16.989947243749878, 7.5590035165610168, -6.154674423116183, 4.1119120658251855, 12.351217703790844, 11.070972687846792, 11.182587746846833, -4.9345619923565645, 9.0054892370887334, -10.841725474869696, 13.902796293412067, -6.7575171884905396, -5.8196703210757335, 1.9284357540668857, 9.9905382141440455, -13.983067199220674, -4.9130522479706453, -8.2369300184767908, 6.8953565265629644, 2.9285103862640871, -2.6303471135655325, -8.3563361642086047, 9.5712349244763715, 4.9728623009661161, -11.045088919587242, -5.7781337596219604, -17.732999074602972, 8.1353860976076646, -11.066240843831284, -1.7079574457159534, -16.411685365171998, -9.0471090651358299, -10.959376227315447, 8.5840398495674126, 6.6373658260736024, 11.422094029020409, 14.85785089306844, 13.185747281780415, 4.2063935223916191, -6.9166135608899282, 10.843153262137262, 5.3913075109409441, -10.744469667642237, -12.491640291445655, 14.141118162062066, 16.425476099516025, 9.8833761863476042, 2.8719064151687883, 14.982021915112442, 1.3588165304065343, -11.657839635726177, 11.066314862965077, -3.0565490195476204, 1.7820159270701772, -13.535333311782074, 4.4026933190218367, -11.097334550496313, -11.322820869044248, 16.418516996530371, 5.8239202459876136, 15.054905601216154, -9.3058742038490152, 8.48902767802557, -8.3853534273227748, -7.9255089736435176, -9.6156735881618811, 11.502594413898008, -6.0542015398269911, 7.1229229147355149, 0.31483632310264387, -11.482093481730768, -9.3225703551629309, 5.8001228713062831, -9.3515917458791051, 7.9778737065172969, 9.7095180444854847, -14.060064536791135, 4.9797253221020545, -6.9210799657794224, 6.6736460552213845, -7.7636429824024606, 10.233132490278882, 8.401747393605044, -10.861100567451366, 13.631509744686715, -15.723791754613185, -8.7931294115815923, -9.9520037489001609, -10.312792052906007, -8.0681893911111917, -15.411052087079765, 10.938779471602952, -8.751795633239853, -9.1302029882284419, -2.3357314769649777, -7.9130658335895596, 2.7508172894969509, -9.1666780515772324, 12.793063537524359, -13.39091818112591, 7.2827402370664842, -10.400778532411657, -1.90854156128735, -4.1272702472088971, 12.696932922959466, -4.0180403457213805, 10.828999052972396, 14.720617452742685, -8.3763729074389719, 3.955093172344033, 0.90932711822659873, -5.6696817865337819, -5.8822086115513805
};
const char HRA_112_112_detected_data[] = {
1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1
};

} // FreeDV
