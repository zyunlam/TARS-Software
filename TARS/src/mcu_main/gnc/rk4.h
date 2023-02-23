#pragma once

#include <array>
#include <cmath>

#include "../EigenArduino-Eigen30/Eigen30.h"
#include "mcu_main/gnc/Atmosphere.h"

using std::array;

class rk4 {
   public:
    rk4();

    array<float, 2> accel(array<float, 2> u, float rho);

    array<float, 2> rk4_step(array<float, 2> state, float dt, float rho);

    array<float, 2> sim_apogee(array<float, 2> state, float dt);

    float cd(float alt, float vel);
    // array<double, 151> poly = {{0,
    //                             0,
    //                             0,
    //                             0,
    //                             0,
    //                             0,
    //                             0,
    //                             0,
    //                             0,
    //                             0,
    //                             0,
    //                             0,
    //                             0,
    //                             0,
    //                             0,
    //                             -0.00000000000000000000000000000000000000000000314461,
    //                             -0.00000000000000000000000000000000000000000001056820,
    //                             -0.00000000000000000000000000000000000000000003019687,
    //                             -0.00000000000000000000000000000000000000000007704372,
    //                             -0.00000000000000000000000000000000000000000017835826,
    //                             -0.00000000000000000000000000000000000000000037122339,
    //                             -0.00000000000000000000000000000000000000000066708435,
    //                             -0.00000000000000000000000000000000000000000089497435,
    //                             -0.00000000000000000000000000000000000000000018652341,
    //                             0.00000000000000000000000000000000000000000452923463,
    //                             0.00000000000000000000000000000000000000002252133750,
    //                             0.00000000000000000000000000000000000000007909889877,
    //                             0.00000000000000000000000000000000000000023785244592,
    //                             0.00000000000000000000000000000000000000064721946801,
    //                             0.00000000000000000000000000000000000000162507072012,
    //                             0.00000000000000000000000000000000000000378114974573,
    //                             0.00000000000000000000000000000000000000807938276390,
    //                             0.00000000000000000000000000000000000001538571557634,
    //                             0.00000000000000000000000000000000000002390366341662,
    //                             0.00000000000000000000000000000000000001998122009594,
    //                             -0.00000000000000000000000000000000000005154859771317,
    //                             -0.00000000000000000000000000000000000036836619651348,
    //                             -0.00000000000000000000000000000000000144815823076849,
    //                             -0.00000000000000000000000000000000000466809435124410,
    //                             -0.00000000000000000000000000000000001345404377599454,
    //                             -0.00000000000000000000000000000000003571811996841365,
    //                             -0.00000000000000000000000000000000008840263006378150,
    //                             -0.00000000000000000000000000000000020390013897460582,
    //                             -0.00000000000000000000000000000000043258491702052162,
    //                             -0.00000000000000000000000000000000081813928333606945,
    //                             -0.00000000000000000000000000000000125802360037397349,
    //                             -0.00000000000000000000000000000000097392013996427856,
    //                             0.00000000000000000000000000000000310343399515662577,
    //                             0.00000000000000000000000000000002096090693898106576,
    //                             0.00000000000000000000000000000008161370547646904257,
    //                             0.00000000000000000000000000000026208013237985893230,
    //                             0.00000000000000000000000000000075497904761522874569,
    //                             0.00000000000000000000000000000200552613510906557360,
    //                             0.00000000000000000000000000000495806281727198586581,
    //                             0.00000000000000000000000000001140718590815672546421,
    //                             0.00000000000000000000000000002401364569498200704843,
    //                             0.00000000000000000000000000004469664389717945228994,
    //                             0.00000000000000000000000000006526293292818941733661,
    //                             0.00000000000000000000000000003638652304169877089656,
    //                             -0.00000000000000000000000000023245124533032718927301,
    //                             -0.00000000000000000000000000134347713820037611194927,
    //                             -0.00000000000000000000000000501626095714016129678212,
    //                             -0.00000000000000000000000001573776213033828266323291,
    //                             -0.00000000000000000000000004442195216883345891619313,
    //                             -0.00000000000000000000000011527054379050515879210897,
    //                             -0.00000000000000000000000027651585789925042193637351,
    //                             -0.00000000000000000000000060818234001986655334512531,
    //                             -0.00000000000000000000000119393714774714749744598259,
    //                             -0.00000000000000000000000192556237706594247426270622,
    //                             -0.00000000000000000000000178227720481147558065092763,
    //                             0.00000000000000000000000353525901387522897914933885,
    //                             0.00000000000000000000002805113876116829800957642964,
    //                             0.00000000000000000000011254938255311482278744615665,
    //                             0.00000000000000000000036304497767041766747357813052,
    //                             0.00000000000000000000103436104702915250883910019659,
    //                             0.00000000000000000000267524730924664163654566364342,
    //                             0.00000000000000000000631564306363270396795988477970,
    //                             0.00000000000000000001338962145030640104106535324544,
    //                             0.00000000000000000002427585673368778749005334317852,
    //                             0.00000000000000000003189318145266884154091181847472,
    //                             -0.00000000000000000000065499453929852287262372393989,
    //                             -0.00000000000000000020338710046932609518034157968140,
    //                             -0.00000000000000000096729556285631687355836548672392,
    //                             -0.00000000000000000332109571855156827258667498502495,
    //                             -0.00000000000000000970372299101872130378039385879080,
    //                             -0.00000000000000002517585307855559328687441482435557,
    //                             -0.00000000000000005835112284480790588626620452450246,
    //                             -0.00000000000000011769890803990919198179373411484463,
    //                             -0.00000000000000018909604030338461769052297304105525,
    //                             -0.00000000000000015100623931873917890151767846966796,
    //                             0.00000000000000048170287264224117961023222597756432,
    //                             0.00000000000000323552018249589884248739919069028492,
    //                             0.00000000000001216963623738738987313103853453908867,
    //                             0.00000000000003678767112229639605527007333020961537,
    //                             0.00000000000009563078057860509211598401891248035354,
    //                             0.00000000000021516989147418571051734288101960693095,
    //                             0.00000000000039889817190574048477382002587654158464,
    //                             0.00000000000049884936905340431429917164534775057003,
    //                             -0.00000000000020942526189187935999093602609157489081,
    //                             -0.00000000000414245225601040749310738837216246902712,
    //                             -0.00000000001789570445050385428595199452049253983216,
    //                             -0.00000000005636076493535205012399175709398690525126,
    //                             -0.00000000014604942156233966023751932855324854251822,
    //                             -0.00000000031173153837571144581390551106514757301325,
    //                             -0.00000000049889718998833044350278879090895486903001,
    //                             -0.00000000028766740658974205165675336395576366238513,
    //                             0.00000000187736351911293997239389874556302756358761,
    //                             0.00000001034453300590629226677060283935694062762423,
    //                             0.00000003464191797877778185037238528956593519581020,
    //                             0.00000008938858455041137313126358441076058092278345,
    //                             0.00000017750172930416809520457442368029932211470623,
    //                             0.00000021575720769800536247432561510334325660664945,
    //                             -0.00000018660145789013343252785474216809413761097858,
    //                             -0.00000220418853725361969662010705461874238153541228,
    //                             -0.00000839635618679585508494344486818405925987462979,
    //                             -0.00002211977951115900022181076645111375000851694494,
    //                             -0.00004073642334724901861459783747321239388838876039,
    //                             -0.00003041590215202065892378772982151957648966345005,
    //                             0.00013507330564461257800534799144998032716102898121,
    //                             0.00075690839307595994892019497513047099346294999123,
    //                             0.00223034394552669087974372530425171134993433952332,
    //                             0.00411366814645865337685126661426693317480385303497,
    //                             0.00190727395441143109111603592964456765912473201752,
    //                             -0.01921304660195650992893234842995298095047473907471,
    //                             -0.08581277934908473903341530331090325489640235900879,
    //                             -0.19550984250010230991456694482621969655156135559082,
    //                             -0.15241962234822362121100525200745323672890663146973,
    //                             0.73715691704171371068099460899247787892818450927734,
    //                             3.49009720639244047646343460655771195888519287109375,
    //                             6.51769703544439504838692300836555659770965576171875,
    //                             -2.73789230353590840039146314666140824556350708007812,
    //                             -50.53373978645147701627138303592801094055175781250000,
    //                             -116.46571370960862168431049212813377380371093750000000,
    //                             51.46970637069863840906691621057689189910888671875000,
    //                             871.57435648196644706331426277756690979003906250000000,
    //                             1168.72820249389701530162710696458816528320312500000000,
    //                             -4221.10437662656931934179738163948059082031250000000000,
    //                             -10902.45880786745874502230435609817504882812500000000000,
    //                             26657.65176031351074925623834133148193359375000000000000,
    //                             53242.00177372424514032900333404541015625000000000000000,
    //                             -269330.56008535361615940928459167480468750000000000000000,
    //                             473564.28085823717992752790451049804687500000000000000000,
    //                             -487887.17901733302278444170951843261718750000000000000000,
    //                             329743.00213737942976877093315124511718750000000000000000,
    //                             -151774.68267442920478060841560363769531250000000000000000,
    //                             47859.42747184166364604607224464416503906250000000000000,
    //                             -10165.17644611340620031114667654037475585937500000000000,
    //                             1388.46553052871922773192636668682098388671875000000000,
    //                             -109.45988611012040792047628201544284820556640625000000,
    //                             3.55595509400277709488591426634229719638824462890625,
    //                             0.53874991700259822202667692181421443819999694824219}};

   private:
    Atmosphere atmo_;
    array<float, 2> y1{{0, 0}};
    array<float, 2> y2{{0, 0}};
    array<float, 2> y3{{0, 0}};
    array<float, 2> y4{{0, 0}};
    array<float, 2> rk4_kp1{{0, 0}};
    // int n = 30;

    /**
     * @brief Approximate the aerodynamic coefficients using a cubic spline interpolation. The coefficients are
     * calculated in Pysim and pasted into this function.
     *
     * @param x_interpolate interpolation points
     * @param x point to evaluate the function at (Mach number)
     * @return float the estimated value of Cd at x
     */
    float approximate_cubic_spline_(float x) {
        Eigen::Matrix<float, 120, 1> c;
        c << 0.0, 15.0, -1.2, 0.68, 15.0, -3.82, 0.29, 0.52, -3.82, 0.9, -0.09, 0.6, 0.9, 0.23, 0.0, 0.57, 0.23, -0.02,
            0.02, 0.56, -0.02, 2.25, 0.02, 0.56, 2.25, -4.17, 0.25, 0.42, -4.17, 6.55, -0.17, 0.72, 6.55, 9.35, 0.49,
            0.19, 9.35, -13.77, 1.42, -0.66, -13.77, -5.0, 0.04, 0.73, -5.0, 1.16, -0.45, 1.28, 1.16, 0.34, -0.34, 1.14,
            0.34, 0.48, -0.3, 1.09, 0.48, 0.76, -0.26, 1.03, 0.76, 0.11, -0.18, 0.91, 0.11, 0.61, -0.17, 0.89, 0.61,
            0.46, -0.11, 0.79, 0.46, -0.05, -0.06, 0.71, -0.05, 0.95, -0.07, 0.72, 0.95, -0.12, 0.03, 0.53, -0.12, 1.35,
            0.02, 0.55, 1.35, -4.08, 0.15, 0.26, -4.08, 1.06, -0.26, 1.19, 1.06, -0.16, -0.15, 0.94, -0.16, 0.17, -0.17,
            0.98, 0.17, 0.07, -0.15, 0.93, 0.07, 0.14, -0.14, 0.91, 0.14, -0.04, -0.13, 0.87, -0.04, 0.0, -0.13, 0.88;

        Eigen::Matrix<float, 30, 1> x_interpolate;
        x_interpolate << 0.01, 0.11310345, 0.2162069 , 0.31931034, 0.42241379,
       0.52551724, 0.62862069, 0.73172414, 0.83482759, 0.93793103,
       1.04103448, 1.14413793, 1.24724138, 1.35034483, 1.45344828,
       1.55655172, 1.65965517, 1.76275862, 1.86586207, 1.96896552,
       2.07206897, 2.17517241, 2.27827586, 2.38137931, 2.48448276,
       2.58758621, 2.69068966, 2.7937931 , 2.89689655, 3.0;


        int i = 0;
        if(x == x_interpolate(x_interpolate.rows()-1, 0)){
            i = x_interpolate.rows() - 1;
        }else{
            i = floor(x * 10.0);
        }

        int ind = 4*i;
        float fa_val = c(ind)/(6*(x_interpolate(i, 0)-x_interpolate(i+1, 0)))*pow((x-x_interpolate(i+1, 0)),3) + 
                    c(ind+1)/(6*(x_interpolate(i+1, 0)-x_interpolate(i, 0)))*pow((x-x_interpolate(i, 0)),3) + 
                    c(ind+2)*x + c(ind+3);

        return fa_val;
    }
};
