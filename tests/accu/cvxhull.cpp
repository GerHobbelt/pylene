#include <mln/accu/accumulators/cvxhull_impl.hpp>

#include <gtest/gtest.h>
#include <fmt/ostream.h>

namespace mln::experimental
{
  void PrintTo(mln::experimental::point2d p, std::ostream* os)
  {
    fmt::print(*os, "({},{})", p.x(), p.y());
  }
}



TEST(CvxHull, null)
{
  auto res = mln::convexhull(nullptr, 0);
  ASSERT_TRUE(res.empty());
}

TEST(CvxHull, SinglePoint)
{
  std::vector<mln::experimental::point2d> points = {{0, 0}, {0, 0}, {0, 0}};
  std::vector<mln::experimental::point2d> ref    = {{0, 0}};

  auto res = mln::convexhull(points.data(), points.size());
  ASSERT_EQ(res, ref);
}


TEST(CvxHull, Line)
{
  std::vector<mln::experimental::point2d> points = {{0, 0}, {0, 0}, {1, 0}, {2, 0}, {3, 0}, {3, 0}};
  std::vector<mln::experimental::point2d> ref    = {{0, 0}, {3, 0}};

  auto res = mln::convexhull(points.data(), points.size());
  ASSERT_EQ(res, ref);
}

TEST(CvxHull, Line_2)
{
  std::vector<mln::experimental::point2d> points = {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 3}, {0, 0}};
  std::vector<mln::experimental::point2d> ref    = {{0, 0}, {0, 3}};

  auto res = mln::convexhull(points.data(), points.size());
  ASSERT_EQ(res, ref);
}


TEST(CvxHull, Square_1)
{
  std::vector<mln::experimental::point2d> points = {{+0, +0}, {+1, +0}, {+2, +0}, {+3, +0}, //
                                                    {+3, +0}, {+3, +1}, {+3, +2}, {+3, +3}, //
                                                    {+0, +3}, {+1, +3}, {+2, +3}, {+3, +3}, //
                                                    {+0, +3}, {+0, +2}, {+0, +1}, {+0, +0}};

  std::vector<mln::experimental::point2d> ref = {{+0, +0}, {+0, +3}, {+3, +3}, {+3, +0}};

  auto res = mln::convexhull(points.data(), points.size());
  ASSERT_EQ(res, ref);
}

TEST(CvxHull, Square_2)
{
  std::vector<mln::experimental::point2d> points = {{-0, +0}, {-1, +0}, {-2, +0}, {-3, +0}, //
                                                    {-3, +0}, {-3, +1}, {-3, +2}, {-3, +3}, //
                                                    {-0, +3}, {-1, +3}, {-2, +3}, {-3, +3}, //
                                                    {-0, +3}, {-0, +2}, {-0, +1}, {-0, +0}};

  std::vector<mln::experimental::point2d> ref = {{-0, +0}, {-0, +3}, {-3, +3}, {-3, +0}};


  auto res = mln::convexhull(points.data(), points.size());
  std::sort(res.begin(), res.end());
  std::sort(ref.begin(), ref.end());

  ASSERT_EQ(res, ref);
}

TEST(CvxHull, Square_3)
{
  std::vector<mln::experimental::point2d> points = {{+0, -0}, {+1, -0}, {+2, -0}, {+3, -0}, //
                                                    {+3, -0}, {+3, -1}, {+3, -2}, {+3, -3}, //
                                                    {+0, -3}, {+1, -3}, {+2, -3}, {+3, -3}, //
                                                    {+0, -3}, {+0, -2}, {+0, -1}, {+0, -0}};

  std::vector<mln::experimental::point2d> ref = {{+0, -0}, {+0, -3}, {+3, -3}, {+3, -0}};


  auto res = mln::convexhull(points.data(), points.size());
  std::sort(res.begin(), res.end());
  std::sort(ref.begin(), ref.end());

  ASSERT_EQ(res, ref);
}

TEST(CvxHull, Square_4)
{
  std::vector<mln::experimental::point2d> points = {{-0, -0}, {-1, -0}, {-2, -0}, {-3, -0}, //
                                                    {-3, -0}, {-3, -1}, {-3, -2}, {-3, -3}, //
                                                    {-0, -3}, {-1, -3}, {-2, -3}, {-3, -3}, //
                                                    {-0, -3}, {-0, -2}, {-0, -1}, {-0, -0}};

  std::vector<mln::experimental::point2d> ref = {{-0, -0}, {-0, -3}, {-3, -3}, {-3, -0}};


  auto res = mln::convexhull(points.data(), points.size());
  std::sort(res.begin(), res.end());
  std::sort(ref.begin(), ref.end());

  ASSERT_EQ(res, ref);
}


// Some dataset generated with https://github.com/qhull/qhull/
TEST(CvxHull, DS_1)
{
  static mln::experimental::point2d points[] = {
      {-473541, -807573}, {-881006, 939647},  {649184, 832006},   {-473563, 826122},  {624358, -411853},
      {-11591, -805774},  {-639320, 949904},  {-959850, -200832}, {622108, -237009},  {596802, 447845},
      {927089, -419198},  {534259, -712781},  {290894, -938273},  {443004, -432747},  {824229, 824427},
      {141195, -933417},  {68498, -757494},   {801118, 390529},   {-383387, 410433},  {151448, -607409},
      {-727982, 814003},  {941893, 394902},   {-884936, 878798},  {-39126, 406001},   {-341703, 997504},
      {-951437, -796575}, {-29192, -625345},  {-174149, -919781}, {-750867, 181842},  {213279, 580061},
      {-913902, 42128},   {46125, -777753},   {308443, 4072},     {433311, 658794},   {356559, 693181},
      {290665, -793339},  {348547, 31278},    {-305853, -464774}, {538949, 111796},   {959451, -505302},
      {-617407, -761789}, {616558, 486037},   {828677, -418830},  {728987, 77290},    {-984482, -185853},
      {366760, 142416},   {-421144, -174878}, {832343, -812421},  {-367127, -310675}, {491873, 907453},
      {-430017, 707462},  {311933, 660941},   {427904, -218157},  {-559604, 732189},  {-92582, -26895},
      {-21951, -927964},  {-291371, 934543},  {859498, -417260},  {-885443, 367027},  {619396, 192109},
      {782420, 136508},   {292533, 607901},   {997495, 906781},   {264855, -575209},  {468865, 214578},
      {415600, 988866},   {-131369, 79591},   {-316442, -437538}, {295132, 277915},   {913893, -202537},
      {-44631, -108931},  {-801547, 398630},  {-220794, -885248}, {-367796, 447685},  {243740, 534759},
      {-306509, 502988},  {-284125, 715102},  {725889, 10428},    {-741476, 18125},   {618907, -30744},
      {-710070, -151309}, {944927, -617758},  {-661212, -989691}, {261431, -136965},  {30816, -78157},
      {410123, 938319},   {334984, 78115},    {878429, -249413},  {118807, 790293},   {448622, -16461},
      {-662599, -308515}, {790902, 693449},   {789747, -717372},  {-864725, 563416},  {-666219, 850997},
      {701115, -359335},  {649903, 922244},   {153863, -27309},   {-982645, 678639},  {-108878, 93711},
      {-993784, -527988}, {103342, 876491},   {-810991, -322611}, {-130278, 409355},  {23354, 518519},
      {753903, 851830},   {714151, 734957},   {423240, -603048},  {564777, 206220},   {-67760, -834455},
      {-687191, 376964},  {-359059, -707964}, {-754097, -105330}, {-288060, 576553},  {130418, -65216},
      {-88125, 876374},   {-783690, 525585},  {-496580, -25598},  {-220973, 103932},  {782029, -430974},
      {620689, -87008},   {-343408, 345041},  {-898433, 29184},   {499434, -5787},    {741357, -12456},
      {658843, -828669},  {566153, -661663},  {-562705, 614565},  {996330, -679037},  {-574560, -629425},
      {-749579, -173270}, {-144472, -139085}, {402350, 293536},   {-545635, -493544}, {-991456, 598724},
      {755269, -189625},  {974140, 369142},   {164136, 629159},   {272442, 929260},   {75068, -338083},
      {-162649, 352252},  {296573, 501342},   {48126, 852610},    {-186714, -101348}, {651894, 378454},
      {679844, 137642},   {-646798, -738268}, {-65407, 701373},   {-28927, -176539},  {903068, -141643},
      {-587176, -675258}, {935237, 527613},   {-411975, -67531},  {-993492, 376171},  {312149, 283485},
      {534675, 283696},   {75058, -493233},   {232238, -767655},  {16358, 923627},    {-598784, 240138},
      {-861, -473297},    {-700075, -166730}, {-230316, -919379}, {-4317, -561721},   {-842040, -164017},
      {-637770, 991600},  {-177449, -383600}, {830574, -548226},  {-37835, 115175},   {-248062, 820475},
      {-283001, -395079}, {-90196, 83342},    {720736, -584451},  {-861461, -580513}, {-683060, -194625},
      {934884, 598496},   {927408, 941270},   {-77735, -493082},  {773852, 129044},   {844741, -446031},
      {-438434, -764102}, {-261745, 843506},  {807720, -648773},  {69349, -452550},   {-8167, 731173},
      {823941, -26232},   {-875291, 977572},  {55134, 633034},    {-599221, 890113},  {126933, -633162},
      {449631, 940996},   {-684727, -212072}, {-299058, -271113}, {-589562, -771063}, {748231, -479687},
      {-91041, -129689},  {309806, 903809},   {318360, 674588},   {-199636, 712366},  {739004, 436455}};

  std::vector<mln::experimental::point2d> ref = {
      {290894, -938273}, {-341703, 997504},  {-951437, -796575}, {832343, -812421},  {997495, 906781},
      {415600, 988866},  {-661212, -989691}, {-982645, 678639},  {-993784, -527988}, {996330, -679037},
      {-991456, 598724}, {-993492, 376171},  {-637770, 991600},  {927408, 941270},   {-875291, 977572}};

  auto res = mln::convexhull(points, sizeof(points) / sizeof(mln::experimental::point2d));
  std::sort(res.begin(), res.end());
  std::sort(ref.begin(), ref.end());

  ASSERT_EQ(res, ref);
}


TEST(CvxHull, DS_2)
{
  static mln::experimental::point2d points[] = {
      {504947, 646967},   {-418350, 796142},  {757023, -708944},  {776029, 720999},   {-172930, -430268},
      {493870, 471402},   {845156, 535278},   {425496, -682176},  {663533, 5691},     {-343055, 277961},
      {-310451, 244570},  {487641, -211656},  {693818, -994331},  {277030, 39111},    {-659607, -8784},
      {367636, 852562},   {-992987, 875718},  {191601, 241863},   {999046, 962267},   {816839, 618601},
      {818864, 639233},   {-412950, -451190}, {853111, 239531},   {-204819, -389176}, {-874311, -539531},
      {107684, -154840},  {-398581, -948656}, {-61598, 715798},   {421615, 81029},    {-147987, 775271},
      {-21504, 577305},   {764401, -718421},  {-506191, 445936},  {843389, 836727},   {874661, 425007},
      {-901774, -109237}, {49688, -892207},   {683472, -882137},  {-81342, 891939},   {813892, -911986},
      {249798, 348764},   {-329851, 188745},  {229780, -82213},   {238831, 33134},    {889007, -467299},
      {102105, 78525},    {-223034, -529130}, {918845, -968224},  {-935496, -879591}, {720274, -361561},
      {-750766, -122810}, {-71053, -195045},  {-122325, 81096},   {985067, 27012},    {-16589, -808496},
      {-391570, 878820},  {331077, 406456},   {-698753, 66044},   {4427, 410857},     {-724756, -966508},
      {-95560, -76417},   {-335597, -384927}, {536081, -89967},   {-71642, -79327},   {754291, -638149},
      {635555, -224409},  {354875, 381634},   {129720, 200254},   {-323080, -8302},   {471810, -293967},
      {-704275, -747276}, {534873, -396531},  {-489043, 661125},  {-472102, -626424}, {-313904, 214844},
      {879088, 830973},   {162353, 670929},   {297796, -948147},  {498383, 330051},   {-835709, 241718},
      {559191, 319889},   {368856, -642726},  {-301865, 550813},  {-477966, 831056},  {-442816, -401393},
      {-210472, 595665},  {-659031, -325843}, {-450118, 869288},  {123232, -842811},  {876110, 781936},
      {4050, 61496},      {-438445, -941087}, {-857287, -415250}, {893637, -647080},  {526309, -319501},
      {145125, -877749},  {-321208, -542212}, {-957129, -472765}, {241426, -348381},  {760607, -482059},
      {40150, 805312},    {879271, -86831},   {637155, 670126},   {810050, 510825},   {-556938, -465102},
      {-975182, 110858},  {-815144, -124950}, {-36664, -208190},  {955847, 918850},   {-888426, 223792},
      {-727726, -882981}, {-266808, -242902}, {-455903, -366400}, {-88891, 6442},     {268278, 954397},
      {549681, 480650},   {286335, 431601},   {-82060, 821783},   {-287569, 824701},  {753040, 339003},
      {-383292, 5745},    {551108, 470256},   {-402391, -977858}, {-855484, -124618}, {-458779, -700429},
      {-108603, 702472},  {452747, -688275},  {158429, 716190},   {998817, -877194},  {-991268, -240362},
      {237145, -312097},  {589421, 400988},   {-599690, -991244}, {154291, -838351},  {-158120, 480974},
      {-278299, 636240},  {-719427, 586503},  {-637790, 665851},  {956733, -188321},  {894783, 613818},
      {442912, 24159},    {44692, -854529},   {-70716, -518548},  {761566, -356294},  {-239648, 235376},
      {-42908, 839823},   {903033, -720841},  {827274, -2733},    {73574, 555441},    {-705519, 348601},
      {944271, 366361},   {-570308, 828738},  {606644, -137709},  {-468553, -974996}, {-760269, 151027},
      {304389, -135023},  {674586, -226888},  {697788, -278728},  {-583304, 406371},  {-121430, -873084},
      {68959, 989876},    {846828, 632602},   {134105, -104649},  {-836250, -859551}, {-471565, 401919},
      {-952258, -597578}, {498272, 460707},   {-899482, 399301},  {-942119, -200302}, {-470717, 656602},
      {-497219, -754313}, {254453, 592843},   {-82117, -136243},  {171784, -822789},  {-609609, 301471},
      {822709, -725726},  {721116, -202782},  {-159783, 532837},  {-613589, -592636}, {-430829, -939175},
      {-709270, -693633}, {117772, -598084},  {134, 258056},      {-848903, 491954},  {277412, 470571},
      {885480, 265626},   {369246, -80284},   {659644, 635895},   {-507747, 290695},  {-289243, 697481},
      {570081, -650855},  {-927013, -299860}, {255527, 635865},   {976198, 952020},   {607022, 225083},
      {963980, -381684},  {-954916, 718439},  {809119, 855692},   {-383788, -324721}, {414030, 609259},


  };

  std::vector<mln::experimental::point2d> ref = {
      {693818, -994331},  {-992987, 875718}, {999046, 962267},   {918845, -968224},  {-935496, -879591},
      {-724756, -966508}, {998817, -877194}, {-991268, -240362}, {-599690, -991244}, {68959, 989876},
  };

  auto res = mln::convexhull(points, sizeof(points) / sizeof(mln::experimental::point2d));
  std::sort(res.begin(), res.end());
  std::sort(ref.begin(), ref.end());

  ASSERT_EQ(res, ref);
}

TEST(CvxHull, DS_3)
{
  static mln::experimental::point2d points[] = {
      {506982, 846792},   {39034, 45176},     {-723495, 220444},  {-992371, -774175}, {444136, 589885},
      {204410, -480342},  {896146, -475973},  {325522, -960126},  {-837627, 7220},    {-654986, -343057},
      {235902, 805475},   {-378431, -298168}, {697210, 7151},     {187901, 58041},    {-499774, 300771},
      {-948948, -975948}, {-755208, -783508}, {-427109, -417238}, {-514927, -371007}, {486560, -393345},
      {-951026, 102307},  {-521246, -578673}, {247941, -862140},  {20208, -365322},   {30281, 927435},
      {-603674, 55419},   {-577228, 527122},  {-652704, 7670},    {916134, -528337},  {246119, 521996},
      {-811700, -246742}, {-999689, 230143},  {20758, 879874},    {40961, 427280},    {-706634, -400233},
      {-717929, -232279}, {90956, 700328},    {417795, -126223},  {573446, -86226},   {807600, -658804},
      {-517704, 943761},  {-204249, -812877}, {-25965, -400925},  {-342795, 640254},  {748055, 561476},
      {732773, -277798},  {-959069, 922033},  {608377, 986982},   {205504, -86824},   {742764, -358961},
      {948268, -464294},  {604163, 170158},   {-155131, 711347},  {-389444, 620988},  {940370, 796335},
      {2227, -565091},    {522877, -1799},    {-237937, 996343},  {-456017, -273749}, {-900164, 944580},
      {-438072, -683498}, {444299, -668678},  {-476084, 463127},  {-218344, 297482},  {-212773, -77869},
      {-746873, -689151}, {-558010, -468350}, {441543, -991158},  {-390412, 350279},  {-859668, -444296},
      {723964, -340981},  {-867131, 122876},  {-829717, 940465},  {401038, 247233},   {-761038, -771373},
      {-472975, 715710},  {930363, 607800},   {-697289, 661834},  {-556455, -343976}, {789138, -959303},
      {998048, 192157},   {-409460, 202361},  {-924318, 985923},  {411362, -243476},  {-96253, 279996},
      {-101454, 864241},  {-693887, -154220}, {26409, -142565},   {-81854, 285625},   {495934, -838170},
      {880327, -340460},  {-112852, -705743}, {570688, -450670},  {-412348, -327390}, {-441036, -495944},
      {673331, 670779},   {-218836, 23774},   {-437811, -289070}, {-403037, 163681},  {986490, -60738},
      {-824812, -620959}, {-462899, 61044},   {-32731, -107661},  {540743, 263839},   {335883, -817697},
      {971144, 17945},    {-405093, -390553}, {-28092, -141183},  {-856868, 618073},  {-42293, -812183},
      {-359635, -386256}, {197103, 707505},   {-966100, 758821},  {-492334, -660787}, {152513, -720790},
      {-320826, -114493}, {-287105, 622301},  {-991726, 56033},   {-249254, 790792},  {843467, 150462},
      {807779, 334391},   {109371, 199235},   {538705, 17071},    {914846, -187454},  {-531829, -449334},
      {46108, 931012},    {-475729, 418822},  {-855398, -675644}, {447318, 68922},    {376821, -771193},
      {556503, -848029},  {-827890, -347234}, {41572, 697433},    {-244143, 683817},  {917688, -424985},
      {-716785, 997956},  {642505, 582336},   {-673855, 514961},  {941847, -383368},  {734335, -25652},
      {874455, 966378},   {-81110, 786778},   {-616612, 610236},  {239850, -838935},  {22980, 224271},
      {-670682, -150070}, {-233347, 143010},  {-436936, 422986},  {-879658, -418025}, {256381, 988753},
      {-32598, 130868},   {-494941, -479486}, {-718784, -601383}, {553454, -92747},   {-803315, 684630},
      {572777, 657029},   {691536, 650230},   {411049, 498519},   {608463, 445175},   {63116, 785596},
      {-481956, -232820}, {985946, 794509},   {-679519, -682099}, {-44145, 51421},    {230167, 422345},
      {346859, -338747},  {682849, 645852},   {833536, -764582},  {-325927, 148132},  {-337795, 676395},
      {166661, -932714},  {-122090, 31018},   {-682484, -500509}, {-46908, -380377},  {996014, 14356},
      {-719508, -773297}, {-802329, -736459}, {330912, -364752},  {-388005, 799929},  {413815, 990831},
      {892794, -806485},  {-587697, 576426},  {-4349, 898463},    {470561, 714490},   {428030, -95833},
      {-668367, 761041},  {815592, -338522},  {454361, 442883},   {-472909, -183308}, {-859752, 142106},
      {368669, 225724},   {-253959, -295546}, {762048, -258115},  {-144123, -269590}, {-991903, -916610},
      {541902, -245105},  {515759, 367279},   {853791, -339918},  {997151, -878816},  {-255803, 723991},
  };

  std::vector<mln::experimental::point2d> ref = {
      {-948948, -975948}, {-999689, 230143}, {-959069, 922033}, {608377, 986982},   {-237937, 996343},
      {441543, -991158},  {789138, -959303}, {998048, 192157},  {-924318, 985923},  {-716785, 997956},
      {874455, 966378},   {985946, 794509},  {413815, 990831},  {-991903, -916610}, {997151, -878816},
  };

  auto res = mln::convexhull(points, sizeof(points) / sizeof(mln::experimental::point2d));
  std::sort(res.begin(), res.end());
  std::sort(ref.begin(), ref.end());

  ASSERT_EQ(res, ref);
}

