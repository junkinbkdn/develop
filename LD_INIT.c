/*#*****************************************************************#*/
/*#        File name   : %M%	#*/
/*#        Summary     : loader initila task						#*/
/*#        Author      : S.Ichikawa									#*/
/*#        Revision    : %I%	#*/
/*#        Date        : %E% %U%	#*/
/*#                                                                 #*/
/*#        Host        : nssun 										#*/
/*#        Directory   : /home/Prober/code/loader 				    #*/
/*#                                                                 #*/
/*#        Copyright (c) Tokyo Electron Limited.                    #*/
/*#                All rights reserved.                             #*/
/*#*****************************************************************#*/

/*#******************************************************************#*/
/*#        Modification                                              #*/
/*#         #1 : 1994.920  semTake ( PALN_SEM,....)追加				 #*/
/*#         #2 : 1995.802  SimpleInitにLdShtOpnSens,LdShtClsSens追加 #*/
/*#         #3 : 1995.918  ｼｬｯﾀｰ有無確認ロジック変更	-> R7.12	 #*/
/*#         #4 : R8.00 1995.1012									 #*/
/*#                     イニシャル時のウェーハ確認を２度行なう。	 #*/
/*#         #5 : R8.02 1995.1204									 #*/
/*#						ｴｱｰ減圧時のｲﾝﾃﾞｸｻｰﾌﾞﾚｰｷ解除の方法を変更		 #*/
/*#						ＱＭＣボード初期化（従来は、ﾌﾞﾚｰｷ解除のみ）	 #*/
/*#			#6 : R9.01 1996.716		Written by M.Togawa				 #*/
/*#						インデクサ停止仕様のため、インデクサ駆動前に #*/
/*#						停止タスクにメッセージの送信を追加。		 #*/
/*#					See MQSend( LdIdxStopTask...)	(in this file )	 #*/
/*#						LD_IDX_STOP					(ldStop.c )		 #*/
/*#			#7 : R9.00-2 1996.716		Written by M.Togawa			 #*/
/*#						ＥＭＣ仕様対応								 #*/
/*#					See LdSpcRdStd		( ldIORd.c )				 #*/
/*#         #8 : R11.00 1997-Apr-22: 外川: @7422mtog-00              #*/
/*#                     Ｆ岩手ウェーハ落下不具合修正                 #*/
/*#         #9 : R011.01 1997-Jul-18: 荻野: @7718kogi-02             #*/
/*#            ウェーハテーブルからのウェーハ搬送時、E2302回避のため #*/
/*#           最初のセンサーリードタイミングをソレノイドＯＮから１秒 #*/
/*#           後にしました。                                         #*/
/*#           尚、イニシャルの時も、ソレノイドＯＮから１秒後に最初の #*/
/*#           センサーリードを行なうように合わせて変更しました。     #*/
/*#                  ＜＜ メカやさんより要求あり。                   #*/
/*#         #10 : R011.01 1997-Aug-05 : 外川 : @7805mtog-00          #*/
/*#             イニシャル時のウェーハ返却処理中、”カセット２の     #*/
/*#             スロット？？にウェーハあるため返却不可”エラーが     #*/
/*#             発生したとき、スライドＳＷを押してもスライドがでない #*/
/*#             ので、カセット内のウェーハを取り出せません。         #*/
/*#             上記のタイミングでスライドＳＷが効くように修正       #*/
/*#             しました。                                           #*/
/*#                 See LdWafReturn  ( in this file )                #*/
/*#                     LD_SLD       ( LD_SLD.c )                    #*/
/*#                                                                  #*/
/*#      R012.00 : 1997-Oct-07 : 荻 野 : @7930kogi-01                #*/
/*#         ダイアグのウェーハ搬送中に、イニシャルが実行された場合に #*/
/*#        ウェーハ搬送が終了するまで、イニシャル動作をしないように  #*/
/*#        変更しました。                                            #*/
/*#        エプソン酒田でピンセットを破損した件の対応です。          #*/
/*#        PrbDActflg = 1 の時は、搬送中と見做して、１秒毎にフラグを #*/
/*#        をチェックし、=0 あるいは、２０秒経過した時点でイニシャル #*/
/*#        動作を開始します。                                        #*/
/*#          See prbDiag.c                                           #*/
/*#                                                                  #*/
/*#      R011.02 : 1997-Oct-08 : 外川 : @7a08mtog-00                 #*/
/*#        イニシャル時にキャリア外に存在するウェーハ返却場所を探す  #*/
/*#        とき、まず履歴の中に CST があるかどうか調べ、なかった     #*/
/*#        に BTBL 、WTBL があるか調べるように変更しました。         #*/
/*#            See ウェーハ返却処理 ( in this file )                 #*/
/*#                LdGetWafReturn, LdGetWafReturnCst ( ldSetGlb.c )  #*/
/*#                                                                  #*/
/*#     R012.02  : 1998-Feb-02 : 荻 野 : @8202kogi-01                #*/
/*#        以下の件について追加、整理しました。                      #*/
/*#        To added or rewrite follows.                              #*/
/*#         ・プロトタイプ宣言、外部参照関数・変数、変数定義等       #*/
/*#           Some kind of definitions which are prototype-functions,#*/
/*#          external-functions/variables, define constants and etc. #*/
/*#                                                                  #*/
/*#   R012.07 : 1998-Jul-23 : 荻 野 : @8723kogi-01                   #*/
/*#         To re-write a definitions.                               #*/
/*#                                                                  #*/
/*#   R012.07 : 1998-Aug-04 : 荻 野 : @8804kogi-01                   #*/
/*#         It corrects warning statement.                           #*/
/*#                                                                  #*/
/*#   R012.07 : 1998-Nov-16 : 石原 : @8b16kish-01                    #*/
/*#         LdInitial()の中で呼ばれているLdPcVacOff()の引数が定義と  #*/
/*#         異なるため、定義と一致するように修正します。             #*/
/*#                                                                  #*/
/*#  R012.08-HostFunction  : 1999-Feb-23 : K.Ishihara : @9223kish-01 #*/
/*#         It corresponded to Host-Function for MV1603 Power-Pc     #*/
/*#                                                                  #*/
/*#  R012.11 : 1999-Jun-02 : K.Ishihara : @9602kish-1768             #*/
/*#         Slide initial sequence is moved in front of PC initial   #*/
/*#         sequence. It is because that the cover opening error     #*/
/*#         occurs in the PC initial when loader initial is executed #*/
/*#         in the condition that slide is not at it's home position.#*/
/*#  R012.12 : 1999-Nov-16 : M.Imai(TSP) : @9b16mima-2936            #*/
/*#         Bug fixed. Pre-align error occurs at Polish-wafer forced #*/
/*#         Lot-end.                                                 #*/
/*#                                                                  #*/
/*#  R012.13 : 2000-May-16 : K.Ishihara : @00516kish-4031            #*/
/*#      1) Changed Prober checks wafer saving sensor before moving  #*/
/*#         pincette Z axis during measurment and initializing.      #*/
/*#        (Added "LdIdxChkWSave()" function to LdPzMove() )         #*/
/*#                                                                  #*/
/*# R012.14 : 2000-Jun-02 : K.Ishihara : @00602kish-4053,            #*/
/*#         Added initializing sequence of subchuck Z motor          #*/
/*#                                                                  #*/
/*# R012.15 : 2000-Sep-06 : K.Ishihara : @00906kish-4735             #*/
/*#         Changed that a message: "A wafer was returned to wafer   #*/
/*#         table " is displayed when a wafer that has no return     #*/
/*#         place.                                                   #*/
/*#         Ex) A wafer on the main chuck in power on initializing.  #*/
/*#         This function is used only for IBM.                      #*/
/*#                                                                  #*/
/*#  R014.02   : 2000-Nov-13 : K.Ishihara : @00a30kish-5130,         #*/
/*#         Added initializing sequence of handling warped wafer.    #*/
/*#                                                                  #*/
/*#  R014.02   : 2000-Dec-26 : K.Ishihara : @00c26kish-4838          #*/
/*#         Correct the software to set loader process mode "LD_DIAG"#*/
/*#         After finishing inisialization in Loader Diagnostics.    #*/
/*#                                                                  #*/
/*#  R014.04      :  2001-Jun-18 : K.Ishihara : @01411kish-5506      #*/
/*#         Added wafer replacement function .                       #*/
/*#         If an wafer is set in a slot in which initialized wafer  #*/
/*#         had been set, initialized wafer is return to WTBL.       #*/
/*#                                                                  #*/
/*# R014.04 : 2001-Jul-16 : K.Ishihara : @01716kish-6008             #*/
/*#         Added cassette cover lock function.                      #*/
/*#         Cassette cover is locked in LdInitialTask before loader  #*/
/*#         initializing.                                            #*/
/*#                                                                  #*/
/*# R014.04 : 2001-Nov-20 : K.Ishihara : @01b20kish-7382             #*/
/*#         Modified warning.                                        #*/
/*#   R014.05 : 2002-07-01 : MWare   @02701wmot-8056                 #*/
/*#    Change taskSpawn and taskDelete by Ty(in taskLock(UnLock))    #*/
/*#																	 #*/
/*# Xxx00-R014.06 : 2003-Dec-18 : H.Yoda : @03c18hyod-11184          #*/
/*#     When "INITIAL" message is received, check whether            #*/
/*#     there is LD_DRV_SEM.If there is no LD_DRV_SEM,               #*/
/*#     create LD_DRV_SEM( with EMPTY ).                             #*/
/*#                                                                  #*/
/*# Xxx00-R014.06-2 : 2005-Jan-27 : H.Yoda @05124hyod-13989          #*/
/*#   Added sendmessage "PC_ORG_END" to LdRootTask                   #*/
/*# Xxx00-R014.08-6 : 2006-Jan-25 : T.Watanabe : @06125twat-15759    #*/
/*#     Remove WARNING for vxw55                                     #*/
/*# Xxx00-R014.08-9 : 2006-Feb-06 : T.Kawasaki : @06306tkaw-16532    #*/
/*#     System Initial sequence improvement                          #*/
/*# Xxx00-R014.08-F : 2006-MAY-27 : H.Yoda : @06527hyod-17006        #*/
/*#       Lower transportation of Bernoulli_Arm by Wafer             #*/
/*# Xxx00-R014.08-K : 2006-Aug-11 : Y.Oda : @06731yoda-18427         #*/
/*#     The state is confirmed initial ahead.                        #*/
/*# Xxx00-R014.08-K : 2006-Aug-24 : Y.Oda : @06824yoda-18603         #*/
/*#     To refer to the sensor, it changes.                          #*/
/*# Xxx00-R014.08-P? : 2006-Dec-27 : M.Togawa : @06c13mtog-17805     #*/
/*#     Adapt Transfer Wafer Func to P-8.                            #*/
/*# Xxx00-R014.08-P : 2007-Feb-19 : Y.Oda : @02418kogi-7783          #*/
/*#   Merge JOB : 19782                                              #*/
/*#     To change the wafer detecte process befor calling            #*/
/*#     SimpleInit(). It will make a operator take a wafer out.      #*/
/*# Xxx00-R014.08-P : 2007-Feb-19 : Y.Oda : @04218mtog-11614         #*/
/*#   Merge JOB : 19782                                              #*/
/*#     added a sequence of vacuum check of SubChack                 #*/
/*# Xxx00-R014.08-P : 2007-Feb-19 : Y.Oda : @04a22kish-14065         #*/
/*#   Merge JOB : 19782                                              #*/
/*#     fixed to turn pc vacuum off after checking wafer existence   #*/
/*#     of subchuck wafer that is transfered from subchuck to pc.    #*/
/*# Xxx00-R014.08-R : 2008-Jan-10 : R.Amemiya : @08110rame-21925     #*/
/*#   Wafer return processing improvement in Bernoulli arm.          #*/
/*# Xxx00-X014.08-S : 2008-Jul-01 : M.Togawa : @08701mtog-22229      #*/
/*#     BERNOULLI Pincette position Auto Change.                     #*/
/*# Xxx00-X014.08-S : 2008-Jul-16 : R.Amemiya : @08612rame-22179     #*/
/*#   Added for Air Pressure Setting.                                #*/
/*# Xxx00-X014.08-T : 2009-Feb-16 : K.Ishii : @08110ykor-21935       #*/
/*#   Merge: added Precio Octo                                       #*/
/*# Xxx00-X014.08-T : 2009-Feb-16 : K.Ishii : @08910ykor-23437       #*/
/*#   Merge: added Precio Octo 2nd Loader and Subchuck-Z             #*/ 
/*# Xxx00-X014.08-T : 2009-Feb-16 : K.Ishii : @08930kisi-22903       #*/ 
/*#   Merge: Correspond H&C shutter for Precio octo                  #*/
/*# Xxx00-X014.08-T : 2009-May-26 : R.Amemiya : @09526rame-23042     #*/
/*#   Wafer detection processing improvement of "Loader Initialize"  #*/
/*#   in "Upper Surface Suction Bernoulli Conveyance Prober".        #*/
/*# Xxx00-X014.08-U : 2009-Sep-07 : K.Ishihara : @09722kish-24778    #*/
/*#   Add the exhaust function for bernoulli pinsette air.           #*/
/*# Xxx00-X015.00-1 : 2010-Jan-25 : T.Ishiyama : @09c11tish-25373    #*/
/*#   BERNOULLI Pincette position Auto Change for Precio octo.       #*/
/*# Xxx00-X015.00-P : 2011-Jun-13 : R.Amemiya : @11530rame-27486     #*/
/*#   It corresponds to Laser Prealign Unit for "Precio Octo".       #*/
/*#     Added TSBATC commands for TSBOITA.                           #*/
/*# Xxx00-X015.00-n : 2012-Feb-03 : R.Shinohara : @08717tomw-22798   #*/
/*#                                               @11922rshi-28393   #*/
/*#     Merge JOB : 22798                                            #*/
/*#     The improvement of userNoGet Function                        #*/
/*# Xxx10-X015.00-2 : 2012-May-30 : R.Amemiya : @12426rame-29489     #*/
/*#   Fault correction which damages a wafer by the wafer return     #*/
/*#   to CST in the case of Bernoulli(Wafer Upper Transfer).         #*/
/*# Xxx10-X015.00-7 : 2012-Aug-08 : Y.Koriyama : @12808ykor-29900    #*/
/*#   fixed bug that isn't absorbed the wafer on arm by vacuum       #*/
/*# Xxx00-X015.01-6 : 2012-Jul-23 : R.Amemiya : @12723rame-29272     #*/
/*#   For "Warped Loader(Phase-2)" of Precio Octo.                   #*/
/*#   (Phase-2:Indexer Air assis, Pincette Rear vacuum)              #*/
/*# Xxx00-X015.01-B : 2013-Jan-22 : R.Amemiya : @12c10rame-29941     #*/
/*#                                             @12b16rame-29941     #*/
/*#   For "Warped 2nd Loader","Back Side Bernoulli Loader" of Precio Octo.#*/
/*# Xxx00-X015.01-s : 2013-Oct-30 : Y.Koriyama : @13a30ykor-30788    #*/
/*#   4/5 inch Laser Prealign Correspond                             #*/
/*# Xxx00-X015.02-d : 2014-Aug-20 : R.Amemiya : @14730rame-33536     #*/
/*#   Responses to "2 line vacuum pincette + Bernoulli sub-chuck".   #*/
/*# Xxx00-X015.02-f : 2014-Sep-01 : R.Amemiya : @14901rame-33022     #*/
/*#   Corresponds to the following of "Precio oct".                  #*/
/*#      3 inches Loader                                             #*/
/*#      3,6-inch dedicated Laser Prealign                           #*/
/*# Xxx00-X016.01-4 : 2014-Dec-10 : S.Ooyama  : @14c10sooy-32785     #*/
/*#    4,5,8inch Quarts Pre-align Unit corresponding.                #*/
/*# Xxx00-X016.02-F : 2016-May-10 : T.Shimura : @16420tshi-37077     #*/
/*#    At the top side Bernoulli, the wafer was to not fall.         #*/
/*# Xxx00-X016.02-S : 2016-May-30 : S.Ooyama  : @16530sooy-36978     #*/
/*#    4,6,8inch Quarts Pre-align Unit corresponding.                #*/
/*# Xxx00-X016.05-H : 2018-Oct-24 : R.Sasamoto : @18a24rsas-41241    #*/
/*#    Bug fix of Cassette map data 0.                               #*/
/*# Xxx00-X016.06-5 : 2019-Jan-15 : R.Amemiya : @18b13rame-41195     #*/
/*#                                 A.Sugino  : @18b02asug-41195     #*/
/*#   Air Pressure Auto Control System is added to Bernoulli         #*/
/*#   Pincette for "Wafer Lower Transfer" and Bernoulli Sub Chuck.   #*/
/*# Xxx00-X016.07-R : 2020-Jan-15 : N.Suzuki  : @20115nsuz-41929     #*/
/*#   orresponds to the following of "Precio oct"                    #*/
/*#     4,6-inch dedicated Laser Prealign.                           #*/
/*# Xxx00-X016.11-4 : 2021-May-24 : T.Hosaka : @21524thos-42311      #*/
/*#    Added New CE Marking specification to Precio octo.            #*/
/*# Xxx00-X016.12-7 : 2022-Apr-05 : E.Takizawa : @22405etak-T15632   #*/
/*#    5,6,8inch Quarts Pre-align Unit corresponding.                #*/
/*# Xxx00-X016.12-L : 2022-Nov-17 : Q Lam : @22b17hlam-T24080        #*/
/*#    Add Pre Check map process when mapping error                  #*/
/*#******************************************************************#*/
/* @06125twat-15759  #include <stdio.h>     */
#include    "condition.h"                    /* @9923kish-01 */
#include    "stdioProb.h"         /* @06125twat-15759 */

#include    "ECCdef.h"
#include    "vxWorks.h"
#include    "iv.h"
#include    "ioLib.h"
#include    "iosLib.h"
#include	"taskLib.h"
#include    "tyLib.h"
#include    "QMC.h"
#include    "MVME316.h"
#include	"ERROR.h"
#include    "LD.h"
#include    "LD_TASKNAME.h"
#include    "LD_GLB.h"
#include    "LD_IO_FUNC.h"
#include    "LD_ERR.h"
#include	"APPL_user.h"
#include    "APPL_main.h"                    /* @01411kish-5506 */
#include    "MSG.h"
#include    "Options.h"
#include    "NP_IO.h"
#include    "ldProto.h"                      /* @8924kish-01 */
#include    "SRAM_DEFINE.h"        /* @06c13mtog-17805 */
#if( P_OCTO )                                 /* @08110ykor-21935 */
#include    "ClsMotor.h"                      /* @08110ykor-21935 */
#include    "ldShDrvr.h"                      /* @08110ykor-21935 */
#include    "mstr.h"                          /* @21524thos-42311 */
#endif                                        /* @08110ykor-21935 */
#include    "macro_def.h"                     /* @08717tomw-22798 */
 
/*------------------------------------------------------ @8202kogi-01 */
/*  The definition of external variables                 @8202kogi-01 */
/*------------------------------------------------------ @8202kogi-01 */
extern	tyMsgDef	LD_INIT_MSG[];

extern intl sortFlag ;

/* インデクサ停止仕様確認のため、次の１行を追加しました。 	*/
/*								Written by M.Togawa  '96.05.23		*/
extern intl ldstop_debug ;

/* ウェーハ落下対応 Apr.22nd 1997 m.togawa */
extern intl LdInt3WtAble( intl, intl ); /* @7422mtog-00 */

extern intl cstTakeDisAble ; 

extern intl wPtrSv;       /* @7918mtog-00 */

extern  intl    IsNewWafPolish( void );     /* @9b16mima-2936 */
extern  unsl    GetSPolishmode( void );     /* @9b16mima-2936 */
extern  intl    SysState;                   /* @01411kish-5506 */

#if( P_OCTO )                               /* @21524thos-42311 */
extern intl LD_STG_SAFE;                    /* @21524thos-42311 */
extern intl Sys_mode;                       /* @21524thos-42311 */
#endif                                      /* @21524thos-42311 */

/*------------------------------------------------------ @8202kogi-01 */
/*  The definition of constant variables                 @8202kogi-01 */
/*------------------------------------------------------ @8202kogi-01 */
#define		LD_INITIAL_PRI		100

#if( P_OCTO )                                         /* @08110ykor-21935 */
#define     LD_INITIAL_STACK    40000                 /* @08110ykor-21935 */
#else                                                 /* @08110ykor-21935 */
/* @06c13mog-17805 #define		LD_INITIAL_STACK	0x2000 */
#define		LD_INITIAL_STACK	0x4000                /* @06c13mtog-17805 */
#endif                                                /* @08110ykor-21935 */

#if( P_OCTO )                                             /* @08110ykor-21935 */
intl    IDXDRIVE_TUNING = 1;                              /* @08110ykor-21935 */
#endif                                                    /* @08110ykor-21935 */

/*------------------------------------------------------ @8202kogi-01 */
/*  The definition of other variables                    @8202kogi-01 */
/*------------------------------------------------------ @8202kogi-01 */

intl	LD_INIT_DEBUG = 0;
#if( P_OCTO )                                         /* @21524thos-42311 */
intl    LD_RECOVER_DEBUG = 0;                         /* @21524thos-42311 */
intl    LdRecoverMtrBootDelay = 2000;                 /* @21524thos-42311 */
#endif                                                /* @21524thos-42311 */
intl	ShtExist = -99999 ;

/* ＥＭＣ仕様対応のため、１行追加  Written by M.Togawa '96.07.16 */
intl ON_EMC ;

/* ウェーハ返却処理中フラグ  @7805mtog-00 */
intl wafRetOnInit=0;      /* @7805mtog-00 */

intl PrbDActflg;          /* @7930kogi-01 */
intl LdDiagFlag = 0;      /* @00c26kish-4838 */

#if LD_SPEC_SMIF                                          /* @9c02mtog-625 */
intl    LdWafRtnSmif( intl, intl );                       /* @9c02mtog-625 */
intl    ldPc2CstSmif( intl, intl, intl );                 /* @9c02mtog-625 */
intl    IDXHome;                                          /* @00b14mtog-5061 */
intl    IDXHomeMil;                                       /* @00b14mtog-5061 */
extern intl   LdGetIdxAddr( intl *, intl * );             /* @9c02mtog-625 */
extern intl   LdErrRstSmf( void );                        /* @9c02mtog-625 */
extern intl   LdMsgSndSmif( intb *, intl );               /* @9c02mtog-625 */
extern intl loadUnload;                                   /* @9c02mtog-625 */
#define    UPC_LPC_DEST  380                              /* @9c02mtog-625 */
#else                                                         /* @06c13mtog-17805 */
intl mapErrCnt = 0 ;                                          /* @06c13mtog-17805 */
extern intl ChkLdBernoulliWithSwitchUnit( VOID ); /* @08701mtog-22229 */
#endif                                                    /* @9c02mtog-625 */
extern intl ChkRtnCalCstWaf( intl, intl );                /* @22b17hlam-T24080 */

/*#-----------------------------------------------------------------#*/
/*#        The definition of external functions                     #*/
/*#-----------------------------------------------------------------#*/
extern int  TyTaskSpawn(char *name, int priority,   /* @02701wmot-8056 */
                   int options, int stackSize,      /* @02701wmot-8056 */
                   FUNCPTR entryPt,int arg1,        /* @02701wmot-8056 */
                   int arg2, int arg3,int arg4,     /* @02701wmot-8056 */
                   int arg5, int arg6, int arg7,    /* @02701wmot-8056 */
                   int arg8, int arg9, int arg10);  /* @02701wmot-8056 */


/*#==================================================================#*/
/*#        Function    : LdInitSetUp								 #*/
/*#        Summary     : set up for initial task ( LD_INIT )	     #*/
/*#        Argument    : #*/
/*#        Return      : #*/
/*#        Extern      : #*/
/*#        Caution     : #*/
/*#			ファイル名を、LD_INIT にしたいが、gcc でパスさせて		 #*/
/*			くれないので、LD_INITIAL とします。						 #*/
/*#==================================================================#*/
STATUS	LdInitSetUp ()
{

    LD_INITmsgSet();
/* @02701wmot-8056     LdInitialTask = taskSpawn( LD_INITIALTASK , LD_INITIAL_PRI , 
        VX_FP_TASK , LD_INITIAL_STACK , (FUNCPTR)LD_INITIAL , 0,0,0,0,0,0,0,0,0,0 );    * VX_SUPERVISOR_MODE */    
/* @02701wmot-8056 */
    LdInitialTask = TyTaskSpawn( LD_INITIALTASK , LD_INITIAL_PRI ,
        VX_FP_TASK , LD_INITIAL_STACK , (FUNCPTR)LD_INITIAL , 0,0,0,0,0,0,0,0,0,0 );

	return( MQCreate( LdInitialTask , 0x00 ) ); /* MSG_Q_FIFO */
}

VOID	LD_INITIAL()
{
	intl	fTask,tTask,msgNum;  
	intl	pNo,ack;
	intb    ackBuf[MSGBUFLIMIT];
	intb	command[MSGBUFLIMIT];
	intl	rtn;
    intl    count ;                  /* @7930kogi-01 */
    intl    cstNo = 0;               /* @12426rame-29489 */
    intl    wafSize = 0;             /* @12426rame-29489 */
    intl    usePc = 0;               /* @16420tshi-37077 */

	FOREVER
	{
		pNo = MsgGetPNo( &fTask , &tTask , &msgNum , &command[0] , 
						 &ack , &ackBuf[0] , LD_INIT_MSG );
        /* When "INITIAL" message is received, check whether there is LD_DRV_SEM.*/
        /* If there is no LD_DRV_SEM, create LD_DRV_SEM( with EMPTY ). */
        if( LD_DRV_SEM == NULL )                                        /* @03c18hyod-11184 */
        {                                                               /* @03c18hyod-11184 */
            LD_DRV_SEM = semBCreate( SEM_Q_FIFO, SEM_EMPTY );           /* @03c18hyod-11184 */
        }                                                               /* @03c18hyod-11184 */
        else                                                            /* @03c18hyod-11184 */
        {                                                               /* @03c18hyod-11184 */
            if( ( semTake( LD_DRV_SEM, NO_WAIT )) == OK )               /* @03c18hyod-11184 */
            {                                                           /* @03c18hyod-11184 */
                if( ( semTake( LD_ERR_SEM, NO_WAIT )) == OK )           /* @03c18hyod-11184 */
                {                                                       /* @03c18hyod-11184 */
                    semGive( LD_ERR_SEM );                              /* @03c18hyod-11184 */
                }                                                       /* @03c18hyod-11184 */
                else                                                    /* @03c18hyod-11184 */
                {                                                       /* @03c18hyod-11184 */
                    semGive( LD_DRV_SEM );                              /* @03c18hyod-11184 */
                    goto SEM_TAKE_ERR;                                  /* @03c18hyod-11184 */
                }                                                       /* @03c18hyod-11184 */
            }                                                           /* @03c18hyod-11184 */
            else                                                        /* @03c18hyod-11184 */
            {                                                           /* @03c18hyod-11184 */
SEM_TAKE_ERR:                                                           /* @03c18hyod-11184 */
                LdSetErr( LD_UNEXPECTED_MSG, 0 );                       /* @03c18hyod-11184 */
                MsgSendAck ( fTask, command, LdGetErr (),               /* @03c18hyod-11184 */
                              ERR_SYSTEM_RESET, taskIdSelf () );        /* @03c18hyod-11184 */
                LdSetStatus ( LD_ERROR, LD_WAIT );                      /* @03c18hyod-11184 */
                continue;                                               /* @03c18hyod-11184 */
            }                                                           /* @03c18hyod-11184 */
        }                                                               /* @03c18hyod-11184 */
		switch( pNo )
		{
			case 1 : 
#if( P_OCTO )                                                                            /* @21524thos-42311 */
                /* インターロック発生状態確認 & インターロックの発生状態フラグのクリア *//* @21524thos-42311 */
                if(( rtn = LdRecoverHInt(0) ))                     /* @10b04tnak-26887 *//* @21524thos-42311 */
                {                                                  /* @10b04tnak-26887 *//* @21524thos-42311 */
                    LdSetErr(rtn,0);                               /* @10b04tnak-26887 *//* @21524thos-42311 */
                    MsgSendAck(fTask,command,                      /* @10b04tnak-26887 *//* @21524thos-42311 */
                               LdGetErr(),                         /* @10b04tnak-26887 *//* @21524thos-42311 */
                               ERR_SYSTEM_RESET,                   /* @10b04tnak-26887 *//* @21524thos-42311 */
                               taskIdSelf());                      /* @10b04tnak-26887 *//* @21524thos-42311 */
                    LdSetStatus(LD_ERROR,LD_WAIT);                 /* @10b04tnak-26887 *//* @21524thos-42311 */
                    break;                                         /* @10b04tnak-26887 *//* @21524thos-42311 */
                }                                                  /* @10b04tnak-26887 *//* @21524thos-42311 */
#endif                                                                                   /* @21524thos-42311 */

				LdInt3WtAble ( INT_WSCH, 0 );
				LdInt3WtAble ( INT_WSAVE, 0 );

#if( !P_OCTO )                                                  /* @08110ykor-21935 */
/* カウンター割り込み禁止  現在使用しているレジスターは、１のみ */
				MtrSetCotInt ( MTR_UPC, 0, 1, 0 );
				MtrSetCotInt ( MTR_LPC, 0, 1, 0 );
#if LD_SPEC_SMIF                                          /* @9c02mtog-625 */
#else                                                     /* @9c02mtog-625 */
				MtrSetCotInt ( MTR_IDX, 0, 1, 0 );
#endif                                                    /* @9c02mtog-625 */
#endif                                                          /* @08110ykor-21935 */

/* ローダダイアグで使用しているデータのクリア */
                LDIAG_ERR_FLAG = 0;
                LdSetStepMode ( 0 );

#if( P_OCTO )                                                         /* @09c11tish-25373 */
                if( chkBernoulliArm() == ERROR )                      /* @09c11tish-25373 */
                {                                                     /* @09c11tish-25373 */
                    LdSetErr( BERNOULLI_ARM_DISCORD, 0 );             /* @09c11tish-25373 */
                    MsgSendAck ( fTask, command, LdGetErr(),          /* @09c11tish-25373 */
                                  ERR_SYSTEM_RESET, taskIdSelf() );   /* @09c11tish-25373 */
                    LdSetStatus ( LD_ERROR, LD_WAIT );                /* @09c11tish-25373 */
                    break ;                                           /* @09c11tish-25373 */
                }                                                     /* @09c11tish-25373 */
                if( chkSwitchUnit() == ERROR )                        /* @09c11tish-25373 */
                {                                                     /* @09c11tish-25373 */
                    LdSetErr( PC_SWITCH_UNIT_DISCORD, 0 );            /* @09c11tish-25373 */
                    MsgSendAck ( fTask, command, LdGetErr(),          /* @09c11tish-25373 */
                                  ERR_SYSTEM_RESET, taskIdSelf() );   /* @09c11tish-25373 */
                    LdSetStatus ( LD_ERROR, LD_WAIT );                /* @09c11tish-25373 */
                    break ;                                           /* @09c11tish-25373 */
                }                                                     /* @09c11tish-25373 */

                if( chkBkSideBer_PcAirCont() == ERROR )               /* @18b13rame-41195 */
                {                                                     /* @18b13rame-41195 */
                    LdSetErr( BERNOULLI_PC_AIRPRES_DISCORD, 0 );      /* @18b13rame-41195 */
                    MsgSendAck( fTask, command, LdGetErr(),           /* @18b13rame-41195 */
                                ERR_SYSTEM_RESET, taskIdSelf() );     /* @18b13rame-41195 */
                    LdSetStatus( LD_ERROR, LD_WAIT );                 /* @18b13rame-41195 */
                    break;                                            /* @18b13rame-41195 */
                }                                                     /* @18b13rame-41195 */

                if( chkBer_ScAirCont() == ERROR )                     /* @18b13rame-41195 */
                {                                                     /* @18b13rame-41195 */
                    LdSetErr( BERNOULLI_SC_AIRPRES_DISCORD, 0 );      /* @18b13rame-41195 */
                    MsgSendAck( fTask, command, LdGetErr(),           /* @18b13rame-41195 */
                                ERR_SYSTEM_RESET, taskIdSelf() );     /* @18b13rame-41195 */
                    LdSetStatus( LD_ERROR, LD_WAIT );                 /* @18b13rame-41195 */
                    break;                                            /* @18b13rame-41195 */
                }                                                     /* @18b13rame-41195 */
#endif    /* P_OCTO */                                                /* @09c11tish-25373 */

/* @18b13rame-41195                if( chkBernoulliArm() == 1 )                              @* @12426rame-29489 */
                if( LdChkBerAirPress_Pc() == 1 )                          /* @18b13rame-41195 */
                {                                                         /* @12426rame-29489 */
                    /* get a size of wafer from SRAM */                   /* @12426rame-29489 */
                    wafSize = SramGet4( SizeOfWaf );                      /* @12426rame-29489 */

                    /* wafer size check */                                /* @12426rame-29489 */
                    rtn = LdChkWafSize( wafSize );                        /* @12426rame-29489 */

                    if( rtn != OK )                                       /* @12426rame-29489 */
                    {                                                     /* @12426rame-29489 */
                        rtn = rtn - ERR_LOADER_TOP;                       /* @12426rame-29489 */
                        LdErrMsg( rtn, ERR_LOADER_ACK, 0 );               /* @12426rame-29489 */
                    }                                                     /* @12426rame-29489 */
                    else                                                  /* @12426rame-29489 */
                    {                                                     /* @12426rame-29489 */
                        cstNo = 0;                  /* cstNoをｸﾘｱ            @12426rame-29489 */

                        while( cstNo < CST_MAX )                          /* @12426rame-29489 */
                        {                                                 /* @12426rame-29489 */
                            /* ｶｾｯﾄﾊﾟﾗﾒｰﾀ(ｴｱ圧力設定)の読み込み              @12426rame-29489 */
                            rtn = LdSetCstDt( cstNo, wafSize, -1 );       /* @12426rame-29489 */

                            if( rtn != OK )                               /* @12426rame-29489 */
                            {                                             /* @12426rame-29489 */
                                rtn = rtn - ERR_LOADER_TOP;               /* @12426rame-29489 */
                                LdErrMsg( rtn, ERR_SYSTEM_RESET,0 );      /* @12426rame-29489 */
                            }                                             /* @12426rame-29489 */

                            if(( OptionInfoGet(PO_2NDLOADER) )            /* @12426rame-29489 */
                            || ( OptionInfoGet(PO_2ND1CST) ))             /* @12426rame-29489 */
                            {                                             /* @12426rame-29489 */
                                cstNo ++;                                 /* @12426rame-29489 */
                            }                                             /* @12426rame-29489 */
                            else                                          /* @12426rame-29489 */
                            {                                             /* @12426rame-29489 */
                                break;                                    /* @12426rame-29489 */
                            }                                             /* @12426rame-29489 */
                        }                                                 /* @12426rame-29489 */
                    }                                                     /* @12426rame-29489 */
                    cstNo = 0;                                                                          /* @16420tshi-37077 */
                    while( usePc < PC_MAX )                                                             /* @16420tshi-37077 */
                    {                                                                                   /* @16420tshi-37077 */
                        rtn = LdPcMaxAirPresCont( usePc, PcAirPresDt.PcAirCst[cstNo].pcAir[usePc] );    /* @16420tshi-37077 */
                        if( rtn != OK )                                                                 /* @16420tshi-37077 */
                        {                                                                               /* @16420tshi-37077 */
                            LdErrMsg( rtn, ERR_LOADER_RETRY,0 );                                        /* @16420tshi-37077 */
                        }                                                                               /* @16420tshi-37077 */
                        else                                                                            /* @16420tshi-37077 */
                        {                                                                               /* @16420tshi-37077 */
                            usePc++;                                                                    /* @16420tshi-37077 */
                        }                                                                               /* @16420tshi-37077 */
                    }                                                                                   /* @16420tshi-37077 */
                }                                                         /* @12426rame-29489 */

/* ステイタスセット */
/* プリアラインＬＥＤ消灯 */
				LdLedWtPaln ( S_OFF, 4 ); /* S_OFF の時はウェーハサイズを無視 */

                LdSetStatus ( LD_RUN, LD_INIT );
				LdSetErr ( 0,0 );			
                for ( count = 0 ; count < 20 ; count++ ) /* @7930kogi-01 */
                {                                        /* @7930kogi-01 */
                    if ( PrbDActflg == 0 ) break ;       /* @7930kogi-01 */
                    TmDelayAux ( TmGetAuxTickS ( 1 ) ) ; /* @7930kogi-01 */
                }                                        /* @7930kogi-01 */

                if( OptionInfoGet(PO_BERNOULLI_ARM) == 1 )                         /* @06527hyod-17006 */
                {                                                                  /* @06527hyod-17006 */
                    if( OptionInfoGet(PO_BERNOULLI_WAFBACK) == 1 )                 /* @06527hyod-17006 */
                    {                                                              /* @06527hyod-17006 */
                        LdErrMsg( LD_ILLEGAL_BERNOULLI_MSG, ERR_SYSTEM_RESET, 0 ); /* @06527hyod-17006 */
                    }                                                              /* @06527hyod-17006 */
                }                                                                  /* @06527hyod-17006 */
				ack = LdInitial();
#if LD_SPEC_SMIF                                          /* @9c02mtog-625 */
#else                                                     /* @9c02mtog-625 */
/* ウェーハサーチＬＥＤ消灯 */
				LdLedWtWSch ( S_OFF );
#endif                                                    /* @9c02mtog-625 */
/* クランプーＳＷ使用ＯＫ ( see LD_INT.c ) */
                cstTakeDisAble = 3 ; 
				LdSetClampDisAble ( 0 );
				if( (ack == CST_NOT_CLAMP1) || (ack == CST_NOT_CLAMP2) )
				{
                   	LdSetErr( ack, 0 );
                   	MsgSendAck ( fTask, command, LdGetErr (),
                                           ERR_SYSTEM_RESET, taskIdSelf () );
                    if( LdDiagFlag )                      /* @00c26kish-4838 */
                    {                                     /* @00c26kish-4838 */
                        LdSetStatus ( LD_ERROR, LD_DIAG );/* @00c26kish-4838 */
                    }                                     /* @00c26kish-4838 */
                    else                                  /* @00c26kish-4838 */
                    {                                     /* @00c26kish-4838 */
                   	    LdSetStatus ( LD_ERROR, LD_WAIT );
                    }                                     /* @00c26kish-4838 */
					break ;
				}
				rtn = MsgSendAck ( fTask, command, LdGetErr (),
						 	ERR_LOADER_INITIAL, taskIdSelf () );
				LdSetMode ( LD_READY );
                if( LdDiagFlag )                          /* @00c26kish-4838 */
                {                                         /* @00c26kish-4838 */
                    LdSetProcs ( LD_DIAG );               /* @00c26kish-4838 */
                }                                         /* @00c26kish-4838 */
                else                                      /* @00c26kish-4838 */
                {                                         /* @00c26kish-4838 */
                    LdSetProcs ( LD_WAIT ); 
                }                                         /* @00c26kish-4838 */
#if LD_SPEC_SMIF                                          /* @9c02mtog-625 */
#else                                                     /* @9c02mtog-625 */

			/* ウェーハ落下対応 Apr.22nd 1997 m.togawa */
				LdInt3WtAble( INT_WSAVE, 1 ); /* @7422mtog-00 */
#endif                                                    /* @9c02mtog-625 */

                PrbDActflg = 0 ;                         /* @7930kogi-01 */

				break;
#if( P_OCTO )                                           /* @21524thos-42311 */
            case 3:  /* LD_DRVER_RECOVER */             /* @13a02rame-28064 *//* @21524thos-42311 */
                if(( rtn = LdRecoverHInt(1) ))          /* @16705twa3-37609 *//* @21524thos-42311 */
                {                                       /* @16115twa3-36508 *//* @21524thos-42311 */
                    LdSetErr(rtn,0);                    /* @16115twa3-36508 *//* @21524thos-42311 */
                    MsgSendAck(fTask,command,           /* @16115twa3-36508 *//* @21524thos-42311 */
                               LdGetErr(),              /* @16115twa3-36508 *//* @21524thos-42311 */
                               ERR_SYSTEM_RESET,        /* @21524thos-42311 */
                               taskIdSelf() );          /* @16115twa3-36508 *//* @21524thos-42311 */
                    LdSetStatus(LD_ERROR,LD_WAIT);      /* @16115twa3-36508 *//* @21524thos-42311 */
                    break;                              /* @16115twa3-36508 *//* @21524thos-42311 */
                }                                       /* @16115twa3-36508 *//* @21524thos-42311 */

                MsgSendAck( fTask, command, LdGetErr(),                       /* @13a02rame-28064 *//* @21524thos-42311 */
                            ERR_LOADER_INITIAL, taskIdSelf() );               /* @13a02rame-28064 *//* @21524thos-42311 */
                break;                                                        /* @13a02rame-28064 *//* @21524thos-42311 */
#endif                                                                                              /* @21524thos-42311 */

			default: logMsg( "pNo=%d,msg=%s\n" , pNo , command );

                PrbDActflg = 0 ;                         /* @7930kogi-01 */

				break;
		}
        /* When Initial process complete, release LD_DRV_SEM. */
        semGive( LD_DRV_SEM );                      /* @03c18hyod-11184 */
	}
}

/*#==================================================================#*/
/*#        Function    : LdInitial                                   #*/
/*#        Summary     : ローダイニシャル							 #*/
/*#        Argument    : 											 #*/
/*#        Return      : error										 #*/
/*#        Extern      : 											 #*/
/*#        Caution     : 											 #*/
/*#		各軸をホーム位置にもどしてから、							 #*/
/*#		アッパー、ロアピンセット、サブチャック上にあるウェーハを     #*/
/*#		カセット、または、テーブルにもどします。返却先がないウェーハ #*/
/*#		は、ウェーハテーブルに返却します。							 #*/
/*#		正常に処理終了後は、全ての軸がホーム位置にあります。		 #*/
/*#		返却においては、プリアラインを行なわないので、				 #*/
/*#		返却したウェーハのフラット位置は、							 #*/
/*#		動作パラメータの「フラット位置設定」で指定した角度とは、     #*/
/*#		なりません。											     #*/	
/*#==================================================================#*/
STATUS 	LdInitial()
{
	intl	rtn;
	intl	dWait;	
	intl	intV;
	intl	cTime;
	intl	ptrSc = -1;
	intl	ptrUPc = -1;
	intl	ptrLPc = -1;
	intl	toId;
	intl	scExistErr = 0;
	intl	uPcExistErr = 0;
	intl	lPcExistErr = 0;
	intl	errRtn;
	intl	usePc;
	intl	home;
	intl	selPaln = 0;	/* LdWafReturn は、プリアラインなしで使用 */
#if LD_SPEC_SMIF                                          /* @9c02mtog-625 */
    intl    setErr=0;                                     /* @9c02mtog-625 */
/* @01b20kish-7382 #endif                                                    @* @9c02mtog-625 */
#else                                                     /* @01b20kish-7382 */
    intl    ptrWTbl = -1;                                 /* @01411kish-5506 */
    intl    setPcErr = 0;                                 /* @06731yoda-18427 */
    intl    ptr;                                              /* @06c13mtog-17805 */
/* @18b13rame-41195    intl    cNo;                                              @* @06c13mtog-17805 */
    intl    cNo = 0;                                      /* @18b13rame-41195 */
#endif                                                    /* @01b20kish-7382 */

	/* SimpleInit は、EWafDt を管理しないので LdWafReturn の fmId の為に
	   現在位置を保存する必要がある。
	   LdWafReturn をプリアラインなしで、使用する場合は、HOME としても
	   問題ありません。*/
	intl	nowUPos = HOME;
	intl	nowLPos = HOME;
	intl	nowSPos = HOME;

#if LD_SPEC_SMIF                                /* @02418kogi-7783 */
#else                                           /* @02418kogi-7783 */
    intl  dummyPtr ;                            /* @02418kogi-7783 */
    intl  sRamPtr ;                             /* @02417kogi-7783 */
    intl  vacErrRetry ;                         /* @02418kogi-7783 */

    intl  wafSize;                          /* @08701mtog-22229 */
#endif                                          /* @02418kogi-7783 */

    intl  scwaf = WAF_EMPTY;                /* @09526rame-23042 */

    intl  scSuctionType;                    /* D-F[14] : @12c10rame-29941 */

    intl  cstNo = 0;                            /* @18a24rsas-41241 */
    intl  sltNo = 0;                            /* @18a24rsas-41241 */

    intl  airContBkPc = 0;                  /* @18b13rame-41195 */
    intl  airContSc = 0;                    /* @18b13rame-41195 */

#if LD_SPEC_SMIF                                          /* @06731yoda-18427 */
#else                                                     /* @06731yoda-18427 */
E_RETRY_LOADER_POS:                                       /* @06731yoda-18427 */
#endif                                                    /* @06731yoda-18427 */

    scSuctionType = ChkSuctionType_Sc();    /* D-F[14] : @12c10rame-29941 */

    /* Air圧力制御機能の設定状態を取得           @18b13rame-41195 */
    airContBkPc = chkBkSideBer_PcAirCont();   /* @18b13rame-41195 */
    airContSc = chkBer_ScAirCont();           /* @18b13rame-41195 */

    wPtrSv = -1;  /* @7918mtog-00 */

#if( !P_OCTO )                                                                 /* @08110ykor-21935 */
/* '95.12.04 1-line modified by c.OGINO
   STG_INT.cで、QMCの制御（MtrContSvOn）を行なうと、ＬＥＤが消灯してしまう。
  このため、LdMtrSetDefault()により再度モーターのデフォルト値をセットします。
  また本関数の中で、全駆動軸のブレーキをオフしています。
  see /home/Prober/code/stage/stgbasic/STG_INT.c
 */
	LdMtrSetDefault () ;
#endif                                                                         /* @08110ykor-21935 */

/* ＥＭＣ対応		Written by M.Togawa '96.07.16 */
	if( LdSpcRdStd() == TRUE )
	{
		ON_EMC = 0 ;							/* 現行Ｐ−８				*/
	}
	else
	{
		ON_EMC = 1 ;							/* ＥＭＣ対応フラグＯＮ		*/
	}

#if LD_SPEC_SMIF                              /* @01716kish-6008 */
#else                                         /* @01716kish-6008 */
    if( OptionInfoGet( PO_CST_CVLOCK ) )      /* @01716kish-6008 */
    {                                         /* @01716kish-6008 */
        LdCvLock( S_ON );                     /* @01716kish-6008 */
    }                                         /* @01716kish-6008 */
#endif                                        /* @01716kish-6008 */

/*  先にピンセットをさげなければ、サブチャックは、ダウンできない */
/* アッパーピンセット上にウェーハあり ？ */
    if( OptionInfoGet( PO_BERNOULLI_ARM ) == 1 )                             /* @06403hyod-16644 */
    {                                                                        /* @06403hyod-16644 */
        rtn = LdPcRdVac( UPPER, S_ON );                                      /* @06403hyod-16644 */
/* @02418kogi-7783    }                                                      @* @06403hyod-16644 */
/* @02418kogi-7783    else                                                   @* @06403hyod-16644 */
/* @02418kogi-7783    {                                                      @* @06403hyod-16644 */
/* @02418kogi-7783		dWait  = PcVac [ UPPER ].dWait;                                          */
/* @02418kogi-7783		intV  = PcVac [ UPPER ].intV;                                            */
/* @02418kogi-7783		cTime = PcVac [ UPPER ].cTime;                                           */
		/* cTime まで監視する必要なし */
/* @02418kogi-7783		rtn = DrvSens( LdPcWtVac , 2 , UPPER , S_ON ,         @* @02418kogi-7783 */
/* @02418kogi-7783			   LdPcRdVac , 2 , UPPER , S_ON , dWait * 2, intV, intV * 3 , 0 ); @* @7718kogi-02 */
/* @02418kogi-7783    }                                                      @* @06403hyod-16644 */

    	if( rtn == OK )
    	{
            if( OptionInfoGet( PO_BERNOULLI_ARM ) == 1 )                         /* @06403hyod-16644 */
            {                                                                    /* @06403hyod-16644 */
                LdPcWtVac( UPPER, S_ON );                                        /* @06403hyod-16644 */
            }                                                                    /* @06403hyod-16644 */
    		if ( LdChkPcWaf (UPPER,WAF_EXIST,&ptrUPc)) 
    		{
    			uPcExistErr=PC_WAF_NOT_RETURN;
    		}
    		else
    		{
    			LdGetEWafNPos ( ptrUPc, &nowUPos );
    			if ( ( nowUPos != SBY ) && ( nowUPos != LID ) )
    			{
    				nowUPos = HOME;
    			}
    		}
    	}
    	else
    	{
            if( ( rtn = LdPcVacOff( UPPER, 1 ) ) )               /* @9223kish-01 */
            {                                             /* @8b16kish-01 */
                return( rtn );                            /* @8b16kish-01 */
            }                                             /* @8b16kish-01 */
    		LdSetPcWaf ( UPPER, WAF_EMPTY );
    	}
    }                                                                           /* @02418kogi-7783 */
    else                                                                        /* @02418kogi-7783 */
    {                                                                           /* @02418kogi-7783 */
        dWait  = PcVac [ UPPER ].dWait;                                         /* @02418kogi-7783 */
        intV  = PcVac [ UPPER ].intV;                                           /* @02418kogi-7783 */
        cTime = PcVac [ UPPER ].cTime;                                          /* @02418kogi-7783 */

#if LD_SPEC_SMIF                                                                /* @02418kogi-7783 */
#else                                                                           /* @02418kogi-7783 */
      do{                                                                       /* @02418kogi-7783 */
        vacErrRetry = 0 ;                                                       /* @02418kogi-7783 */
#endif                                                                          /* @02418kogi-7783 */
        if( airContBkPc == 1 )                                      /* @18b13rame-41195 */
        {                                                           /* @18b13rame-41195 */
            rtn = LdPcRdVac( UPPER, S_ON );                         /* @18b13rame-41195 */
        }                                                           /* @18b13rame-41195 */
        else                                                        /* @18b13rame-41195 */
        {                                                           /* @18b13rame-41195 */
            if( WarpedLoader == TRUE )                                      /* @12723rame-29272 */
            {                                                               /* @12723rame-29272 */
                rtn = DrvSens( LdPcWtVac, 2, UPPER, S_ON,                   /* @12723rame-29272 */
                               LdPcRdVac, 2, UPPER, S_ON,                   /* @12723rame-29272 */
                               dWait, intV, cTime, 0 );                     /* @12723rame-29272 */
            }                                                               /* @12723rame-29272 */
            else                                                            /* @12723rame-29272 */
            {                                                               /* @12723rame-29272 */
                /* cTime まで監視する必要なし */
                rtn = DrvSens( LdPcWtVac , 2 , UPPER , S_ON ,
                       LdPcRdVac , 2 , UPPER , S_ON , dWait * 2, intV, intV * 3 , 0 );  /* @02418kogi-7783 */
            }                                                               /* @12723rame-29272 */
        }                                                           /* @18b13rame-41195 */

        if( rtn == OK )                                                         /* @02418kogi-7783 */
        {                                                                       /* @02418kogi-7783 */
            if( airContBkPc == 1 )                                                /* @18b13rame-41195 */
            {                                                                     /* @18b13rame-41195 */
                /* PC上にWaferは有るが吸着しているかは不明の為、                     @18b13rame-41195 */
                /* Air圧力制御し、AirをONして吸着させます。                          @18b13rame-41195 */
                if(( rtn = LdPcAirPresCont_OnOff( UPPER, S_ON, AIR_PRES_CONT,     /* @18b13rame-41195 */
                                  PcAirPresDt.PcAirCst[cNo].pcAir[UPPER],         /* @18b13rame-41195 */
                                  PcAirPresDt.PcAirCst[cNo].pcAirTime[UPPER] )))  /* @18b13rame-41195 */
                {                                                                 /* @18b13rame-41195 */
                    return( rtn );       /* Air圧力制御NG */                      /* @18b13rame-41195 */
                }                                                                 /* @18b13rame-41195 */
            }                                                                     /* @18b13rame-41195 */
            else                                                                  /* @18b13rame-41195 */
            {                                                                     /* @18b13rame-41195 */
#if( P_OCTO )                                                           /* @12723rame-29272 */
                /* バキュームセンサがOFFのピンセットバキュームソレノイド       @12723rame-29272 */
                /* を検出し、そのバキュームをOFFします。                       @12723rame-29272 */
                rtn = LdVacOffPcSol_Off( UPPER );                           /* @12723rame-29272 */
                if( rtn != OK )                                             /* @12723rame-29272 */
                {                                                           /* @12723rame-29272 */
                    return( LdSetErr( rtn, 0 ) );                           /* @12723rame-29272 */
                }                                                           /* @12723rame-29272 */
#endif                                                                  /* @12723rame-29272 */
            }                                                                        /* @18b13rame-41195 */

            if ( LdChkPcWaf (UPPER,WAF_EXIST,&ptrUPc))                          /* @02418kogi-7783 */
            {                                                                   /* @02418kogi-7783 */
                uPcExistErr=PC_WAF_NOT_RETURN;                                  /* @02418kogi-7783 */
            }                                                                   /* @02418kogi-7783 */
            else                                                                /* @02418kogi-7783 */
            {                                                                   /* @02418kogi-7783 */
                LdGetEWafNPos ( ptrUPc, &nowUPos );                             /* @02418kogi-7783 */
                if ( ( nowUPos != SBY ) && ( nowUPos != LID ) )                 /* @02418kogi-7783 */
                {                                                               /* @02418kogi-7783 */
                    nowUPos = HOME;                                             /* @02418kogi-7783 */
                }                                                               /* @02418kogi-7783 */
            }                                                                   /* @02418kogi-7783 */
        }                                                                       /* @02418kogi-7783 */
        else                                                                    /* @02418kogi-7783 */
        {                                                                       /* @02418kogi-7783 */
#if LD_SPEC_SMIF                                                                /* @02418kogi-7783 */
            if( ( rtn = LdPcVacOff( UPPER, 1 ) ) )               /* @9223kish-01 */
            {                                             /* @8b16kish-01 */
                return( rtn );                            /* @8b16kish-01 */
            }                                             /* @8b16kish-01 */
            LdSetPcWaf ( UPPER, WAF_EMPTY );
#else                                                                           /* @02418kogi-7783 */
            /* バキュームをオフる                                                  @02417kogi-7783 */
            if( ( rtn = LdPcVacOff( UPPER, 1 ) ) )                              /* @02417kogi-7783 */
            {                                                                   /* @02417kogi-7783 */
                return( rtn );                                                  /* @02417kogi-7783 */
            }                                                                   /* @02417kogi-7783 */
            /* ＳＲＡＭのポインターをチェック復活！！                              @02423kogi-7783 */
            sRamPtr = SramGet4( UpcWaf ) ;                                      /* @02423kogi-7783 */
            if( ( OK == LdChkPcWaf( UPPER, WAF_EXIST, &dummyPtr ) ) || /* ﾎﾟｲﾝﾀ有  @02423kogi-7783 */
                ( ( sRamPtr != 0x00 ) &&      /* Pointer exists in SRAM.   */   /* @02507kogi-7783 */
                  ( sRamPtr != 0xff ) &&                                        /* @02507kogi-7783 */
                  ( 0 != sRamPtr )       )                                      /* @02507kogi-7783 */
              )                                                                 /* @02423kogi-7783 */
            {                                                                   /* @02417kogi-7783 */
            /* "アッパアーム上のウェーハをバキュームセンサーで検出できません。"出力 */
            /* 状態は"ウェーハを取り除いてください。"を記述しておくこと             */
                errRtn = LdErrMsg( UPC_WAF_EXIST_CHECK,                         /* @02509kogi-7783 */
                                                   ERR_LOADER_ACK_VACCHK, 0 ) ; /* @02509kogi-7783 */
                if( ERR_RETRY_MSG == errRtn )                                   /* @02417kogi-7783 */
                {                                                               /* @02417kogi-7783 */
                /* "イニシャル処理を続行します。ウェーハがアッパアーム上に無い事を確認してください"出力 */
                /* 状態は"ウェーハを破損する可能性があります。"を記述しておくこと                 */

                    errRtn = LdErrMsg( UPC_WAF_BREAK_WARN,                      /* @02417kogi-7783 */
                                                  ERR_LOADER_ACK_VACCHK2, 0 ) ; /* @02509kogi-7783 */
                    if( ERR_RETRY_MSG == errRtn )                               /* @02417kogi-7783 */
                    {                                                           /* @02417kogi-7783 */
                        vacErrRetry = 1 ;                                       /* @02418kogi-7783 */
                    }                                                           /* @02417kogi-7783 */
                    else                                                        /* @02418kogi-7783 */
                    {                                                           /* @02418kogi-7783 */
/* SRAMﾎﾟｲﾝﾀｰがにある場合は、SRAMの中身をｸﾘｱしないとまずい！！ */
/* ｸﾘｱしないとSRAMに残っているので、再度ｲﾆｼｬﾙするとウェーハがﾎﾝﾄに無いのに */
/* またこの処理を行って、余計なエラーが出る。 */
                        SramSet4( UpcWaf, 0 ) ;                                 /* @02418kogi-7783 */
                        LdSetPcWaf ( UPPER, WAF_EMPTY ) ;                       /* @02418kogi-7783 */
                        break ;  /* exitting do-while loop!! */                 /* @02418kogi-7783 */
                    }                                                           /* @02418kogi-7783 */
                }                                                               /* @02417kogi-7783 */
            }                                                                   /* @02417kogi-7783 */
            /* それでも無かったら、ないものと判断します                            @02417kogi-7783 */
            else                                                                /* @02417kogi-7783 */
            {                                                                   /* @02417kogi-7783 */
                LdSetPcWaf ( UPPER, WAF_EMPTY );                                /* @02417kogi-7783 */
            }                                                                   /* @02417kogi-7783 */
#endif                                                                          /* @02418kogi-7783 */
        }                                                                       /* @02418kogi-7783 */
#if LD_SPEC_SMIF                                                                /* @02418kogi-7783 */
#else                                                                           /* @02418kogi-7783 */
      } while( vacErrRetry ) ;                                                  /* @02417kogi-7783 */
#endif                                                                          /* @02418kogi-7783 */
    }                                                                           /* @02418kogi-7783 */

/* ロアピンセット上にウェーハあり ？ */
    if( OptionInfoGet( PO_BERNOULLI_ARM ) == 1 )                             /* @06403hyod-16644 */
    {                                                                        /* @06403hyod-16644 */
        rtn = LdPcRdVac( LOWER, S_ON );                                      /* @06403hyod-16644 */
/* @02418kogi-7783    }                                                      @* @06403hyod-16644 */
/* @02418kogi-7783    else                                                   @* @06403hyod-16644 */
/* @02418kogi-7783    {                                                      @* @06403hyod-16644 */
/* @02418kogi-7783		dWait  = PcVac [ LOWER ].dWait;                                          */
/* @02418kogi-7783		intV  = PcVac [ LOWER ].intV;                                            */
/* @02418kogi-7783		cTime = PcVac [ LOWER ].cTime;                                           */
		/* cTime まで監視する必要なし */
/* @02418kogi-7783		rtn = DrvSens( LdPcWtVac , 2 , LOWER , S_ON ,                            */
/* @02418kogi-7783			   LdPcRdVac , 2 , LOWER , S_ON , dWait * 2, intV, intV * 3 , 0 ); @* @7718kogi-02 */
/* @02418kogi-7783        }                                                  @* @06403hyod-16644 */

    	if( rtn == OK )
    	{
            if( OptionInfoGet( PO_BERNOULLI_ARM ) == 1 )                         /* @06403hyod-16644 */
            {                                                                    /* @06403hyod-16644 */
                LdPcWtVac( LOWER, S_ON );                                        /* @06403hyod-16644 */
            }                                                                    /* @06403hyod-16644 */
    		if ( LdChkPcWaf (LOWER,WAF_EXIST,&ptrLPc)) 
    		{
    			lPcExistErr=PC_WAF_NOT_RETURN;
    		}
    		else
    		{
                LdGetEWafNPos ( ptrLPc, &nowLPos );
                if ( ( nowLPos != SBY ) && ( nowLPos != LID ) )
                {
                    nowLPos = HOME;
                }
            }
    	}
    	else
    	{
            if( ( rtn = LdPcVacOff( LOWER, 1 ) ) )               /* @9223kish-01 */
            {                                             /* @8b16kish-01 */
                return( rtn );                            /* @8b16kish-01 */
            }                                             /* @8b16kish-01 */
    		LdSetPcWaf ( LOWER, WAF_EMPTY );
    	}
    }                                                                           /* @02418kogi-7783 */
    else                                                                        /* @02418kogi-7783 */
    {                                                                           /* @02418kogi-7783 */
        dWait  = PcVac [ LOWER ].dWait;                                         /* @02418kogi-7783 */
        intV  = PcVac [ LOWER ].intV;                                           /* @02418kogi-7783 */
        cTime = PcVac [ LOWER ].cTime;                                          /* @02418kogi-7783 */

#if LD_SPEC_SMIF                                                                /* @02418kogi-7783 */
#else                                                                           /* @02418kogi-7783 */
      do{                                                                       /* @02418kogi-7783 */
        vacErrRetry = 0 ;                                                       /* @02418kogi-7783 */
#endif                                                                          /* @02418kogi-7783 */
        if( airContBkPc == 1 )                                      /* @18b13rame-41195 */
        {                                                           /* @18b13rame-41195 */
            rtn = LdPcRdVac( LOWER, S_ON );                         /* @18b13rame-41195 */
        }                                                           /* @18b13rame-41195 */
        else                                                        /* @18b13rame-41195 */
        {                                                           /* @18b13rame-41195 */
            if( WarpedLoader == TRUE )                                      /* @12723rame-29272 */
            {                                                               /* @12723rame-29272 */
                rtn = DrvSens( LdPcWtVac, 2, LOWER, S_ON,                   /* @12723rame-29272 */
                               LdPcRdVac, 2, LOWER, S_ON,                   /* @12723rame-29272 */
                               dWait, intV, cTime, 0 );                     /* @12723rame-29272 */
            }                                                               /* @12723rame-29272 */
            else                                                            /* @12723rame-29272 */
            {                                                               /* @12723rame-29272 */
                /* cTime まで監視する必要なし */
                rtn = DrvSens( LdPcWtVac , 2 , LOWER , S_ON ,
                       LdPcRdVac , 2 , LOWER , S_ON , dWait * 2, intV, intV * 3 , 0 );  /* @02418kogi-7783 */
            }                                                               /* @12723rame-29272 */
        }                                                           /* @18b13rame-41195 */

        if( rtn == OK )                                                         /* @02418kogi-7783 */
        {                                                                       /* @02418kogi-7783 */
            if( airContBkPc == 1 )                                                /* @18b13rame-41195 */
            {                                                                     /* @18b13rame-41195 */
                /* PC上にWaferは有るが吸着しているかは不明の為、                     @18b13rame-41195 */
                /* Air圧力制御し、AirをONして吸着させます。                          @18b13rame-41195 */
                if(( rtn = LdPcAirPresCont_OnOff( LOWER, S_ON, AIR_PRES_CONT,     /* @18b13rame-41195 */
                                  PcAirPresDt.PcAirCst[cNo].pcAir[LOWER],         /* @18b13rame-41195 */
                                  PcAirPresDt.PcAirCst[cNo].pcAirTime[LOWER] )))  /* @18b13rame-41195 */
                {                                                                 /* @18b13rame-41195 */
                    return( rtn );       /* Air圧力制御NG */                      /* @18b13rame-41195 */
                }                                                                 /* @18b13rame-41195 */
            }                                                                     /* @18b13rame-41195 */
            else                                                                  /* @18b13rame-41195 */
            {                                                                     /* @18b13rame-41195 */
#if( P_OCTO )                                                           /* @12723rame-29272 */
                /* バキュームセンサがOFFのピンセットバキュームソレノイド       @12723rame-29272 */
                /* を検出し、そのバキュームをOFFします。                       @12723rame-29272 */
                rtn = LdVacOffPcSol_Off( LOWER );                           /* @12723rame-29272 */
                if( rtn != OK )                                             /* @12723rame-29272 */
                {                                                           /* @12723rame-29272 */
                    return( LdSetErr( rtn, 0 ) );                           /* @12723rame-29272 */
                }                                                           /* @12723rame-29272 */
#endif                                                                  /* @12723rame-29272 */
            }                                                                     /* @18b13rame-41195 */

            if ( LdChkPcWaf (LOWER,WAF_EXIST,&ptrLPc))                          /* @02418kogi-7783 */
            {                                                                   /* @02418kogi-7783 */
                lPcExistErr=PC_WAF_NOT_RETURN;                                  /* @02418kogi-7783 */
            }                                                                   /* @02418kogi-7783 */
            else                                                                /* @02418kogi-7783 */
            {                                                                   /* @02418kogi-7783 */
                LdGetEWafNPos ( ptrLPc, &nowLPos );                             /* @02418kogi-7783 */
                if ( ( nowLPos != SBY ) && ( nowLPos != LID ) )                 /* @02418kogi-7783 */
                {                                                               /* @02418kogi-7783 */
                    nowLPos = HOME;                                             /* @02418kogi-7783 */
                }                                                               /* @02418kogi-7783 */
            }                                                                   /* @02418kogi-7783 */
        }                                                                       /* @02418kogi-7783 */
        else                                                                    /* @02418kogi-7783 */
        {                                                                       /* @02418kogi-7783 */
#if LD_SPEC_SMIF                                                                /* @02418kogi-7783 */
        if( ( rtn = LdPcVacOff( LOWER, 1 ) ) )               /* @9223kish-01 */
        {                                             /* @8b16kish-01 */
            return( rtn );                            /* @8b16kish-01 */
        }                                             /* @8b16kish-01 */
        LdSetPcWaf ( LOWER, WAF_EMPTY );
#else                                                                           /* @02418kogi-7783 */
            /* バキュームをオフる                                                  @02417kogi-7783 */
            if( ( rtn = LdPcVacOff( LOWER, 1 ) ) )                              /* @02417kogi-7783 */
            {                                                                   /* @02417kogi-7783 */
                return( rtn );                                                  /* @02417kogi-7783 */
            }                                                                   /* @02417kogi-7783 */

            /* ポインターのチェックと、ＳＲＡＭのポインターをチェック              @02417kogi-7783 */
/**     sRamPtr = SramGet4( LpcWaf ) ;                                           * @02417kogi-7783 */
/**     if( OK == LdChkPcWaf( LOWER, WAF_EXIST, &dummyPtr ) ) @* @02417kogi-7783 *//* @02423kogi-7783 */
            /* ＳＲＡＭのポインターをチェック復活！！                              @02423kogi-7783 */
            sRamPtr = SramGet4( LpcWaf ) ;                                      /* @02423kogi-7783 */
            if( ( OK == LdChkPcWaf( LOWER, WAF_EXIST, &dummyPtr ) ) || /* ﾎﾟｲﾝﾀ有 */ /* @02423kogi-7783 */
                ( ( sRamPtr != 0x00 ) &&      /* Pointer exists in SRAM.   */   /* @02507kogi-7783 */
                  ( sRamPtr != 0xff ) &&                                        /* @02507kogi-7783 */
                  ( 0 != sRamPtr )       )                                      /* @02507kogi-7783 */
              )                                                                 /* @02423kogi-7783 */
            {                                                                   /* @02417kogi-7783 */
                /* "ロアアーム上のウェーハをバキュームセンサーで検出できません。"出力 */
                /* 状態は"ウェーハを取り除いてください。"を記述しておくこと             */
                errRtn = LdErrMsg( LPC_WAF_EXIST_CHECK,                         /* @02509kogi-7783 */
                                                   ERR_LOADER_ACK_VACCHK, 0 ) ; /* @02509kogi-7783 */
                if( ERR_RETRY_MSG == errRtn )                                   /* @02417kogi-7783 */
                {                                                               /* @02417kogi-7783 */
                    /* "イニシャル処理を続行します。ウェーハがアッパアーム上に無い事を確認してください"出力 */
                    /* 状態は"ウェーハを破損する可能性があります。"を記述しておくこと                 */

                    errRtn = LdErrMsg( LPC_WAF_BREAK_WARN,                      /* @02417kogi-7783 */
                                                  ERR_LOADER_ACK_VACCHK2, 0 ) ; /* @02509kogi-7783 */
                    if( ERR_RETRY_MSG == errRtn )                               /* @02417kogi-7783 */
                    {                                                           /* @02417kogi-7783 */
                        vacErrRetry = 1 ;                                       /* @02418kogi-7783 */
                    }                                                           /* @02417kogi-7783 */
                    else                                                        /* @02418kogi-7783 */
                    {                                                           /* @02418kogi-7783 */
                        SramSet4( LpcWaf, 0 ) ;                                 /* @02418kogi-7783 */
                        LdSetPcWaf ( LOWER, WAF_EMPTY ) ;                       /* @02418kogi-7783 */
                        break ;  /* exitting do-while loop!! */                 /* @02418kogi-7783 */
                    }                                                           /* @02418kogi-7783 */
                }                                                               /* @02417kogi-7783 */
            }                                                                   /* @02417kogi-7783 */
            /* それでも無かったら、ないものと判断します                            @02417kogi-7783 */
            else                                                                /* @02417kogi-7783 */
            {                                                                   /* @02417kogi-7783 */
                LdSetPcWaf ( LOWER, WAF_EMPTY );                                /* @02417kogi-7783 */
            }                                                                   /* @02417kogi-7783 */
#endif                                                                          /* @02418kogi-7783 */
        }                                                                       /* @02418kogi-7783 */
#if LD_SPEC_SMIF                                                                /* @02418kogi-7783 */
#else                                                                           /* @02418kogi-7783 */
      } while( vacErrRetry ) ;                                                  /* @02417kogi-7783 */
#endif                                                                          /* @02418kogi-7783 */
    }                                                                           /* @02418kogi-7783 */

/* サブチャックにウェーハあり ？ */
	dWait = ScVac.dWait;
	intV  = ScVac.intV;
	cTime = ScVac.cTime;

#if LD_SPEC_SMIF                                                            /* @02418kogi-7783 */
#else                                                                       /* @02418kogi-7783 */
  do{                                                                       /* @02418kogi-7783 */
    vacErrRetry = 0 ;                                                       /* @02418kogi-7783 */
#endif                                                                      /* @02418kogi-7783 */
    if( airContSc == 1 )                                        /* @18b13rame-41195 */
    {                                                           /* @18b13rame-41195 */
        rtn = LdScRdVac( S_ON );                                /* @18b13rame-41195 */
    }                                                           /* @18b13rame-41195 */
    else                                                        /* @18b13rame-41195 */
    {                                                           /* @18b13rame-41195 */
    	/* cTime まで監視する必要なし */
    	rtn = DrvSens( LdScWtVac , 1 , S_ON , 
    			LdScRdVac , 1 , S_ON , dWait, intV, intV * 3 , 0 );
    }                                                           /* @18b13rame-41195 */

	if( rtn == OK )
	{
        if( airContSc == 1 )                                                        /* @18b13rame-41195 */
        {                                                                           /* @18b13rame-41195 */
            /* SC上にWaferは有るが吸着しているかは不明の為、                           @18b13rame-41195 */
            /* Air圧力制御し、AirをONして吸着させます。                                @18b13rame-41195 */
            if(( rtn = LdScAirPresCont_OnOff( S_ON, AIR_PRES_CONT,                  /* @18b13rame-41195 */
                                       PcAirPresDt.PcAirCst[cNo].scPcAir,           /* @18b13rame-41195 */
                                       PcAirPresDt.PcAirCst[cNo].scPcAirTime )))    /* @18b13rame-41195 */
            {                                                                       /* @18b13rame-41195 */
                return( rtn );       /* Air圧力制御NG */                            /* @18b13rame-41195 */
            }                                                                       /* @18b13rame-41195 */
        }                                                                           /* @18b13rame-41195 */

		if ( LdChkScWaf ( WAF_EXIST, &ptrSc ) ) 
		{
			scExistErr = SC_WAF_NOT_RETURN;
		}
        else
        {  
            LdGetEWafNPos ( ptrSc, &nowSPos );
            if ( ( nowSPos != SBY ) && ( nowSPos != LID ) )
            {
                nowSPos = HOME;
            }
        }

#if( !LD_SPEC_SMIF )                                                      /* @08701mtog-22229 */
        if( OptionInfoGet( PO_BERNOULLI_ARM ) == 1 )                         /* @06403hyod-16644 */
        {                                                                    /* @06403hyod-16644 */
/* @09526rame-23042            rtn = LdReturnScWaf_Brnl( scExistErr );                          @* @06403hyod-16644 */
/* @09526rame-23042            if( rtn != OK )                                                  @* @06403hyod-16644 */
/* @09526rame-23042            {                                                                @* @06403hyod-16644 */
/* @09526rame-23042                return( LdSetErr(rtn, 0) );                                  @* @06403hyod-16644 */
/* @09526rame-23042            }                                                                @* @06403hyod-16644 */
            scwaf = WAF_EXIST;                                             /* @09526rame-23042 */
        }                                                                    /* @06403hyod-16644 */
        else                                                                 /* @06403hyod-16644 */
        {                                                                    /* @06403hyod-16644 */
#endif                                                                    /* @08701mtog-22229 */
            if( OptionInfoGet(PO_SUBCHUCKZ) )                             /* @00602kish-4053 */
            {                                                             /* @00602kish-4053 */
                if((LdPcRdVac( UPPER, 1 ) == OK)                          /* @00602kish-4053 */
                    ||(LdPcRdVac( LOWER, 1) == OK))                       /* @00602kish-4053 */
                {                                                         /* @00602kish-4053 */
/* D-F[14] : @12c10rame-29941                    if( OptionInfoGet(PO_BERNOULLI_WAFBACK) == 0 )        @* @06527hyod-17006 */
                    if( OptionInfoGet(PO_BERNOULLI_WAFBACK) == 0 )        /* @14730rame-33536 */
                    {                                                     /* @14730rame-33536 */
                        if( scSuctionType == VACUUM_SC )                      /* D-F[14] : @12c10rame-29941 */
                        {                                                     /* @06527hyod-17006 */
                            if(( rtn = LdScVacOff( 1 )))                          /* @00602kish-4053 */
                            {                                                     /* @00602kish-4053 */
                                return( rtn );                                    /* @00602kish-4053 */
                            }                                                     /* @00602kish-4053 */
                        }                                                     /* @06527hyod-17006 */
                        else                                              /* @14730rame-33536 */
                        {                                                 /* @14730rame-33536 */
                            LdScWtVac( S_OFF );                           /* @14730rame-33536 */
                        }                                                 /* @14730rame-33536 */
                    }                                                     /* @14730rame-33536 */
/* @08910ykor-23437                    if(( MtrRdStatus( MTR_UPC, 2 ) & MTR2_ORG_BIT )       @* @00602kish-4053 */
/* @08910ykor-23437                       && ( MtrRdStatus ( MTR_LPC, 2 ) & MTR2_ORG_BIT ))  @* @00602kish-4053 */
                    if( ( LdChkBothPcHm() == OK ) )                  /* @08910ykor-23437 */
                    {                                                     /* @00602kish-4053 */
                        if( OptionInfoGet(PO_BERNOULLI_WAFBACK) == 1 )    /* @06527hyod-17006 */
                        {                                                 /* @06527hyod-17006 */
                            if( scSuctionType == BERNOULLI_SC )           /* D-F[14] : @12c10rame-29941 */
                            {                                             /* D-F[14] : @12c10rame-29941 */
                                if( airContSc == 1 )                                                        /* @18b13rame-41195 */
                                {                                                                           /* @18b13rame-41195 */
                                    /* PC&SC上にWaferは有るが(PC<->SC間の受け渡し)、SCが吸着している           @18b13rame-41195 */
                                    /* かは不明の為、Air圧力制御し、AirをOFFして吸着させます。                 @18b13rame-41195 */
                                    if(( rtn = LdScAirPresCont_OnOff( S_OFF, AIR_PRES_CONT,                 /* @18b13rame-41195 */
                                                               PcAirPresDt.PcAirCst[cNo].scPcAir,           /* @18b13rame-41195 */
                                                               PcAirPresDt.PcAirCst[cNo].scPcAirTime )))    /* @18b13rame-41195 */
                                    {                                                                       /* @18b13rame-41195 */
                                        return( rtn );       /* Air圧力制御NG */                            /* @18b13rame-41195 */
                                    }                                                                       /* @18b13rame-41195 */
                                }                                                                           /* @18b13rame-41195 */
                                else                                                                        /* @18b13rame-41195 */
                                {                                                                           /* @18b13rame-41195 */
                                    LdScWtVac ( S_OFF );                          /* @06527hyod-17006 */
                                }                                                                           /* @18b13rame-41195 */
                            }                                             /* D-F[14] : @12c10rame-29941 */
                            else                                          /* @14730rame-33536 */
                            {                                             /* @14730rame-33536 */
                                if(( rtn = LdScVacOff( 1 )))              /* @14730rame-33536 */
                                {                                         /* @14730rame-33536 */
                                    return( rtn );                        /* @14730rame-33536 */
                                }                                         /* @14730rame-33536 */
                            }                                             /* @14730rame-33536 */

                            if( airContBkPc == 1 )                                                  /* @18b13rame-41195 */
                            {                                                                       /* @18b13rame-41195 */
                                if(( rtn = LdPcAirPresCont_OnOff( UPPER, S_ON, AIR_PRES_CONT,       /* @18b13rame-41195 */
                                                  PcAirPresDt.PcAirCst[cNo].pcScAir[UPPER],         /* @18b13rame-41195 */
                                                  PcAirPresDt.PcAirCst[cNo].pcScAirTime[UPPER] )))  /* @18b13rame-41195 */
                                {                                                                   /* @18b13rame-41195 */
                                    return( rtn );       /* Air圧力制御NG */                        /* @18b13rame-41195 */
                                }                                                                   /* @18b13rame-41195 */
                                if(( rtn = LdPcAirPresCont_OnOff( LOWER, S_ON, AIR_PRES_CONT,       /* @18b13rame-41195 */
                                                  PcAirPresDt.PcAirCst[cNo].pcScAir[LOWER],         /* @18b13rame-41195 */
                                                  PcAirPresDt.PcAirCst[cNo].pcScAirTime[LOWER] )))  /* @18b13rame-41195 */
                                {                                                                   /* @18b13rame-41195 */
                                    return( rtn );       /* Air圧力制御NG */                        /* @18b13rame-41195 */
                                }                                                                   /* @18b13rame-41195 */
                            }                                                                       /* @18b13rame-41195 */
                            else                                                                    /* @18b13rame-41195 */
                            {                                                                       /* @18b13rame-41195 */
                                LdPcWtVac ( UPPER, S_ON );                    /* @06527hyod-17006 */
                                LdPcWtVac ( LOWER, S_ON );                    /* @06527hyod-17006 */
                            }                                                                       /* @18b13rame-41195 */
                        }                                                 /* @06527hyod-17006 */
E_RETRY_SCZ:                                                          /* @00602kish-4053 */
                        if( ( rtn = LdScZGoOrg() ) )                      /* @00602kish-4053 */
                        {                                                 /* @00602kish-4053 */
                            errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 );/* @00602kish-4053 */
                            if ( errRtn == ERR_RETRY_MSG ) goto E_RETRY_SCZ; /* @00602kish-4053 */
                            if ( errRtn == LD_DIAG_ERR ) return ( rtn );  /* @00602kish-4053 */
                        }                                                 /* @00602kish-4053 */
                        scwaf = WAF_EMPTY;                                /* @12808ykor-29900 : P-F2 */
                    }                                                     /* @00602kish-4053 */
                    else                                                  /* @12808ykor-29900 : P-F2 */
                    {                                                     /* @12808ykor-29900 : P-F2 */
                        scwaf = WAF_EXIST;                                /* @12808ykor-29900 : P-F2 */
                    }                                                     /* @12808ykor-29900 : P-F2 */
                }                                                         /* @00602kish-4053 */
                else                                                      /* @12808ykor-29900 : P-F2 */ 
                {                                                         /* @12808ykor-29900 : P-F2 */
                    scwaf = WAF_EXIST;                                    /* @12808ykor-29900 : P-F2 */
                }                                                         /* @12808ykor-29900 : P-F2 */
            }                                                             /* @00602kish-4053 */
#if( !LD_SPEC_SMIF )                                                      /* @08701mtog-22229 */
        }                                                                    /* @06403hyod-16644 */
#endif                                                                    /* @08701mtog-22229 */
	}
	else
	{
#if LD_SPEC_SMIF                                                            /* @02418kogi-7783 */
        if( ( rtn = LdScVacOff( 1 ) ) )                      /* @9223kish-01 */
        {                                                    /* @9223kish-01 */
            return( rtn );                                   /* @9223kish-01 */
        }                                                    /* @9223kish-01 */
        LdSetScWaf ( WAF_EMPTY );
#else                                                                       /* @02418kogi-7783 */
      /* バキュームをオフる                                                    @02417kogi-7783 */
        if( ( rtn = LdScVacOff( 1 ) ) )                                     /* @02417kogi-7783 */
        {                                                                   /* @02417kogi-7783 */
            return( rtn );                                                  /* @02417kogi-7783 */
        }                                                                   /* @02417kogi-7783 */

/* @12808ykor-29900 : P-F2       if( OptionInfoGet( PO_BERNOULLI_ARM ) == 1 )                          @* @09526rame-23042 */
/* @12808ykor-29900 : P-F2       {                                                                     @* @09526rame-23042 */
        scwaf = WAF_EMPTY;                                                /* @09526rame-23042 */
/* @12808ykor-29900 : P-F2       }                                                                     @* @09526rame-23042 */

      /* ポインターのチェックと、ＳＲＡＭのポインターをチェック                @02417kogi-7783 */
/**     sRamPtr = SramGet4( SckWaf ) ;                                       * @02417kogi-7783 */
/**     if( OK == LdChkScWaf ( WAF_EXIST, &dummyPtr ) )              @* @02417kogi-7783 *//* @02423kogi-7783 */
      /* ＳＲＡＭのポインターをチェック復活！！                                              @02423kogi-7783 */
        sRamPtr = SramGet4( SckWaf ) ;                                                    /* @02423kogi-7783 */
        if( ( OK == LdChkScWaf( WAF_EXIST, &dummyPtr ) ) ||        /* ﾎﾟｲﾝﾀ有 */          /* @02423kogi-7783 */
            ( ( sRamPtr != 0x00 ) &&      /* Pointer exists in SRAM.   */    /* @02507kogi-7783 */
              ( sRamPtr != 0xff ) &&                                         /* @02507kogi-7783 */
              ( 0 != sRamPtr )       )                                       /* @02507kogi-7783 */
          )                                                                               /* @02423kogi-7783 */
        {                                                                   /* @02417kogi-7783 */
            /* "サブチャック上のウェーハをバキュームセンサーで検出できません。"出力 */
            /* 状態は"ウェーハを取り除いてください。"を記述しておくこと             */
            errRtn = LdErrMsg( SC_WAF_EXIST_CHECK,                          /* @02509kogi-7783 */
                                               ERR_LOADER_ACK_VACCHK, 0 ) ; /* @02509kogi-7783 */
            if( ERR_RETRY_MSG == errRtn )                                   /* @02417kogi-7783 */
            {                                                               /* @02417kogi-7783 */
                /* "イニシャル処理を続行します。ウェーハがアッパアーム上に無い事を確認してください"出力 */
                /* 状態は"ウェーハを破損する可能性があります。"を記述しておくこと                 */

                errRtn = LdErrMsg( SC_WAF_BREAK_WARN,                       /* @02417kogi-7783 */
                                              ERR_LOADER_ACK_VACCHK2, 0 ) ; /* @02509kogi-7783 */
                if( ERR_RETRY_MSG == errRtn )                               /* @02417kogi-7783 */
                {                                                           /* @02417kogi-7783 */
                    vacErrRetry = 1 ;                                       /* @02418kogi-7783 */
                }                                                           /* @02417kogi-7783 */
                else                                                        /* @02418kogi-7783 */
                {                                                           /* @02418kogi-7783 */
                    SramSet4( SckWaf, 0 ) ;                                 /* @02418kogi-7783 */
                    LdSetScWaf( WAF_EMPTY ) ;                               /* @02418kogi-7783 */
                    break ;  /* exitting do-while loop!! */                 /* @02418kogi-7783 */
                }                                                           /* @02418kogi-7783 */
            }                                                               /* @02417kogi-7783 */
        }                                                                   /* @02417kogi-7783 */
      /* それでも無かったら、ないものと判断します                              @02417kogi-7783 */
        else                                                                /* @02417kogi-7783 */
        {                                                                   /* @02417kogi-7783 */
            LdSetScWaf ( WAF_EMPTY );                                       /* @02417kogi-7783 */
        }                                                                   /* @02417kogi-7783 */
#endif                                                                      /* @02418kogi-7783 */
	}
#if LD_SPEC_SMIF                                                            /* @02418kogi-7783 */
#else                                                                       /* @02418kogi-7783 */
  } while( vacErrRetry ) ;                                                  /* @02417kogi-7783 */

    if( OptionInfoGet( PO_BERNOULLI_ARM ) == 1 )                          /* @09526rame-23042 */
    {                                                                     /* @09526rame-23042 */
        if( LdChkScWaf( WAF_EXIST, &ptrSc ) )                             /* @09526rame-23042 */
        {                                                                 /* @09526rame-23042 */
            scExistErr = SC_WAF_NOT_RETURN;    /* no pointer */           /* @09526rame-23042 */
        }                                                                 /* @09526rame-23042 */

        rtn = LdScVacOff(0);                                              /* @09526rame-23042 */

        if( rtn == OK )                                                   /* @09526rame-23042 */
        {                                                                 /* @09526rame-23042 */
            rtn = LdScZInitial();                                         /* @09526rame-23042 */
        }                                                                 /* @09526rame-23042 */

        if( rtn != OK )                                                   /* @09526rame-23042 */
        {                                                                 /* @09526rame-23042 */
            return( LdSetErr(rtn, 0) );                                   /* @09526rame-23042 */
        }                                                                 /* @09526rame-23042 */
    }                                                                     /* @09526rame-23042 */
#endif                                                                      /* @02418kogi-7783 */

    if(OptionInfoGet(PO_WARPHANDLE))                          /* @00a30kish-5130 */
    {                                                         /* @00a30kish-5130 */
        /* Stop blowing air to Sc --drive only */             /* @00a30kish-5130 */
        rtn = DrvSens(LdScWtBlow,1,S_OFF,0,0,0,0,0,0);        /* @00a30kish-5130 */
        if( rtn )                                             /* @00a30kish-5130 */
        {                                                     /* @00a30kish-5130 */
            return( LdSetErr( WARP_AIR_NOT_ON, 0 ) );         /* @00a30kish-5130 */
        }                                                     /* @00a30kish-5130 */
                                                              /* @00a30kish-5130 */
        /* Stop blowing air to WTbl --drive only */           /* @00a30kish-5130 */
        rtn = DrvSens(LdWTblWtBlow,1,S_OFF,0,0,0,0,0,0);      /* @00a30kish-5130 */
        if( rtn )                                             /* @00a30kish-5130 */
        {                                                     /* @00a30kish-5130 */
            return( LdSetErr( WARP_AIR_NOT_ON, 0 ) );         /* @00a30kish-5130 */
        }                                                     /* @00a30kish-5130 */
    }                                                         /* @00a30kish-5130 */

#if( P_OCTO )                                               /* @12723rame-29272 */
    if( WarpedLoader == TRUE )                              /* @12723rame-29272 */
    {                                                       /* @12723rame-29272 */
        /* Indexer Air Assist OFF. */                       /* @12723rame-29272 */
/* @12b16rame-29941        rtn = LdIdxAirBlowOnOff( S_OFF );                   @* @12723rame-29272 */
        rtn = LdIdxAirBlowOnOff( CST_0, S_OFF );            /* @12b16rame-29941 */

        if(( OptionInfoGet( PO_2NDLOADER ) )                /* @12b16rame-29941 */
        || ( OptionInfoGet( PO_2ND1CST ) ))                 /* @12b16rame-29941 */
        {                                                   /* @12b16rame-29941 */
            if( rtn == OK )                                 /* @12b16rame-29941 */
            {                                               /* @12b16rame-29941 */
                rtn = LdIdxAirBlowOnOff( CST_1, S_OFF );    /* @12b16rame-29941 */
            }                                               /* @12b16rame-29941 */
        }                                                   /* @12b16rame-29941 */

        if( rtn != OK )                                     /* @12723rame-29272 */
        {                                                   /* @12723rame-29272 */
            return( LdSetErr( rtn, 0 ) );                   /* @12723rame-29272 */
        }                                                   /* @12723rame-29272 */
    }                                                       /* @12723rame-29272 */
#endif                                                      /* @12723rame-29272 */

#if LD_SPEC_SMIF                                                      /* @06731yoda-18427 */
#else                                                                 /* @06731yoda-18427 */
    if( OptionInfoGet(PO_BERNOULLI_ARM) == 0 )                        /* @06731yoda-18427 */
    {                                                                 /* @06731yoda-18427 */
        if( OptionInfoGet(PO_BERNOULLI_WAFBACK) == 0 )                /* @06731yoda-18427 */
        {                                                             /* @06731yoda-18427 */
/* @06824yoda-18603 if( LdPartDt.idxPos == IDX_HM )                   @* @06731yoda-18427 */
/* @08110ykor-21935            if(( MtrRdStatus( MTR_IDX, 2 ) & MTR2_NORG_BIT ) != 0 )   @* @06824yoda-18603 */
            if( LdIdxRdOrg() == 1 )                                     /* @08110ykor-21935 */
            {                                                         /* @06731yoda-18427 */
                if( LdPzRdSolDn() == TRUE )                           /* @06731yoda-18427 */
                {                                                     /* @06731yoda-18427 */
                    if(( ptrUPc >= 0 ) || ( uPcExistErr == PC_WAF_NOT_RETURN )) /* @06731yoda-18427 */
                    {                                                 /* @06731yoda-18427 */
                        if(( rtn = LdPcVacOff( UPPER, 1 )))           /* @06731yoda-18427 */
                        {                                             /* @06731yoda-18427 */
                            return( rtn );                            /* @06731yoda-18427 */
                        }                                             /* @06731yoda-18427 */
                        setPcErr = LOADER_POS_ERR;                    /* @06731yoda-18427 */
                    }                                                 /* @06731yoda-18427 */
                    if(( ptrLPc >= 0 ) || ( lPcExistErr == PC_WAF_NOT_RETURN )) /* @06731yoda-18427 */
                    {                                                 /* @06731yoda-18427 */
                        if(( rtn = LdPcVacOff( LOWER, 1 )))           /* @06731yoda-18427 */
                        {                                             /* @06731yoda-18427 */
                            return( rtn );                            /* @06731yoda-18427 */
                        }                                             /* @06731yoda-18427 */
                        setPcErr = LOADER_POS_ERR;                    /* @06731yoda-18427 */
                    }                                                 /* @06731yoda-18427 */
                    if( setPcErr == LOADER_POS_ERR )                  /* @06731yoda-18427 */
                    {                                                 /* @06731yoda-18427 */
                        if(( rtn = LdCstRls( 1, 1 )))                 /* @06731yoda-18427 */
                        {                                             /* @06731yoda-18427 */
                            return( rtn );                            /* @06731yoda-18427 */
                        }                                             /* @06731yoda-18427 */
                        errRtn = LdErrMsg( setPcErr, ERR_LOADER_RETRY, 0 ); /* @06731yoda-18427 */
                        if( errRtn == ERR_RETRY_MSG )                 /* @06731yoda-18427 */
                        {                                             /* @06731yoda-18427 */
                            ptrUPc = -1;                              /* @06731yoda-18427 */
                            ptrLPc = -1;                              /* @06731yoda-18427 */
                            uPcExistErr = 0;                          /* @06731yoda-18427 */
                            lPcExistErr = 0;                          /* @06731yoda-18427 */
                            setPcErr = 0;                             /* @06731yoda-18427 */
                            goto E_RETRY_LOADER_POS;                  /* @06731yoda-18427 */
                        }                                             /* @06731yoda-18427 */
                    }                                                 /* @06731yoda-18427 */
                }                                                     /* @06731yoda-18427 */
            }                                                         /* @06731yoda-18427 */
        }                                                             /* @06731yoda-18427 */
    }                                                                 /* @06731yoda-18427 */

    if( ChkLdBernoulliWithSwitchUnit() == TRUE )               /* @08701mtog-22229 */
    {                                                          /* @08701mtog-22229 */
        wafSize = SramGet4( SizeOfWaf );                       /* @08701mtog-22229 */
        if( (wafSize != POS_6INCH) && (wafSize != POS_8INCH) ) /* @08701mtog-22229 */
        {                                                      /* @08701mtog-22229 */
            wafSize = POS_6INCH;                               /* @08701mtog-22229 */
/* @12426rame-29489            SramSet4( SizeOfWaf, wafSize );                    @* @08701mtog-22229 */
        }                                                      /* @08701mtog-22229 */

#if( P_OCTO )                                                  /* @09c11tish-25373 */
        rtn = OLdPcChgUnitInitChk();                           /* @09c11tish-25373 */
#else                                                          /* @09c11tish-25373 */
        rtn = LdUnitGetReady4Down( wafSize, DO_UP );           /* @08701mtog-22229 */
#endif    /* P_OCTO */                                         /* @09c11tish-25373 */
        if( rtn != OK )                                        /* @08701mtog-22229 */
        {                                                      /* @08701mtog-22229 */
            return( rtn );                                     /* @08701mtog-22229 */
        }                                                      /* @08701mtog-22229 */
    }                                                          /* @08701mtog-22229 */

#endif                                                                /* @06731yoda-18427 */

/* @09526rame-23042    if( ( rtn = SimpleInit() ) )                             @* @9223kish-01 */
    rtn = SimpleInit( scExistErr, scwaf );                      /* @09526rame-23042 */
    if( rtn != OK )                                             /* @09526rame-23042 */
    {                                                        /* @9223kish-01 */
        return( rtn );                                       /* @9223kish-01 */
    }                                                        /* @9223kish-01 */

#if( P_OCTO )                                               /* @08110ykor-21935 */
    if( ( rtn = OLdIdxDriveAndTuning() ) )                  /* @08110ykor-21935 */
    {                                                       /* @08110ykor-21935 */
        return( rtn );                                      /* @08110ykor-21935 */
    }                                                       /* @08110ykor-21935 */
#endif                                                      /* @08110ykor-21935 */

    if( OptionInfoGet(PO_BERNOULLI_WAFBACK) == 1 )      /* @06527hyod-17006 */
    {                                                   /* @06527hyod-17006 */
        uPcExistErr = 0;                                /* @06527hyod-17006 */
        lPcExistErr = 0;                                /* @06527hyod-17006 */
        scExistErr = 0;                                 /* @06527hyod-17006 */
    }                                                   /* @06527hyod-17006 */
    else                                                    /* @12808ykor-29900 : P-F2 */
    {                                                       /* @12808ykor-29900 : P-F2 */
        if( OptionInfoGet(PO_BERNOULLI_ARM) == 0 )          /* @12808ykor-29900 : P-F2 */
        {                                                   /* @12808ykor-29900 : P-F2 */
            scwaf = WAF_EMPTY;                              /* @12808ykor-29900 : P-F2 */ 
        }                                                   /* @12808ykor-29900 : P-F2 */
    }                                                       /* @12808ykor-29900 : P-F2 */

/*  '95.10.12 82-lines added by c.ogino			*/
/*	各軸の原点駆動が終了した後、再度ウェーハが各ポジションに
	残っているか否かを確認します。
	各軸の駆動前だけで確認を行なっていると、ｳｪｰﾊを認識できない場合
	原点駆動時に、ｳｪｰﾊ破損に繋がる為（特にｻﾌﾞﾁｬｯｸ駆動）
	再度確認することにしました。
 */
	if( (sortFlag != 1) && (sortFlag != (-1)) )
	{
/* アッパーピンセット上にウェーハあり ？ */
/* @18b13rame-41195        if( OptionInfoGet( PO_BERNOULLI_ARM ) == 1 )                             @* @06403hyod-16644 */
        if( LdChkBerAirPress_Pc() == 1 )                                         /* @18b13rame-41195 */
        {                                                                        /* @06403hyod-16644 */
            rtn = LdPcRdVac( UPPER, S_ON );                                      /* @06403hyod-16644 */
        }                                                                        /* @06403hyod-16644 */
        else                                                                     /* @06403hyod-16644 */
        {                                                                        /* @06403hyod-16644 */
			dWait  = PcVac [ UPPER ].dWait;
			intV  = PcVac [ UPPER ].intV;
			cTime = PcVac [ UPPER ].cTime;

            if( WarpedLoader == TRUE )                                      /* @12723rame-29272 */
            {                                                               /* @12723rame-29272 */
                rtn = DrvSens( LdPcWtVac, 2, UPPER, S_ON,                   /* @12723rame-29272 */
                               LdPcRdVac, 2, UPPER, S_ON,                   /* @12723rame-29272 */
                               dWait, intV, cTime, 0 );                     /* @12723rame-29272 */
            }                                                               /* @12723rame-29272 */
            else                                                            /* @12723rame-29272 */
            {                                                               /* @12723rame-29272 */
    			/* cTime まで監視する必要なし */
    			rtn = DrvSens( LdPcWtVac , 2 , UPPER , S_ON ,
    		   	LdPcRdVac , 2 , UPPER , S_ON , dWait * 2, intV, intV * 3 , 0 ); /* @7718kogi-02 */
            }                                                               /* @12723rame-29272 */
        }                                                                        /* @06403hyod-16644 */
		if( rtn == OK )
		{
#if( P_OCTO )                                                           /* @12723rame-29272 */
            /* バキュームセンサがOFFのピンセットバキュームソレノイド       @12723rame-29272 */
            /* を検出し、そのバキュームをOFFします。                       @12723rame-29272 */
            rtn = LdVacOffPcSol_Off( UPPER );                           /* @12723rame-29272 */
            if( rtn != OK )                                             /* @12723rame-29272 */
            {                                                           /* @12723rame-29272 */
                return( LdSetErr( rtn, 0 ) );                           /* @12723rame-29272 */
            }                                                           /* @12723rame-29272 */
#endif                                                                  /* @12723rame-29272 */
            /* 通常アーム（ベルヌーイ以外）の場合は、サブチャックのウェーハポインタをピンセットに */ /* @12808ykor-29900 */
            /* 移動しているため、移動履歴が残っている場合は以下のシーケンスには入りません         */ /* @12808ykor-29900 */
			if ( LdChkPcWaf (UPPER,WAF_EXIST,&ptrUPc)) 
			{
/* @06527hyod-17006                if(OptionInfoGet(PO_SUBCHUCKZ))                  @* @00602kish-4053 */
                if( ( OptionInfoGet( PO_SUBCHUCKZ ) ) ||         /* @06527hyod-17006 */
                    ( OptionInfoGet( PO_BERNOULLI_WAFBACK ) ) )  /* @06527hyod-17006 */
                {                                                /* @00602kish-4053 */
                    if( ptrSc >= 0 )                             /* @00602kish-4053 */
                    {                                            /* @00602kish-4053 */
                        ptrUPc = ptrSc;                          /* @00602kish-4053 */
                        nowUPos = nowSPos;                       /* @00602kish-4053 */
                        ptrSc = -1;                              /* @00602kish-4053 */
                        nowSPos = HOME;                          /* @00602kish-4053 */
                        LdSetPcWaf( UPPER, ptrUPc );             /* @00602kish-4053 */
                        if( OptionInfoGet(PO_BERNOULLI_WAFBACK) )    /* @06527hyod-17006 */
                        {                                            /* @06527hyod-17006 */
                            uPcExistErr=PC_WAF_NOT_RETURN;           /* @06527hyod-17006 */
                        }                                            /* @06527hyod-17006 */
                    }                                            /* @00602kish-4053 */
                    else if( scExistErr )                        /* @00602kish-4053 */
                    {                                            /* @00602kish-4053 */
                        uPcExistErr=PC_WAF_NOT_RETURN;           /* @00602kish-4053 */
                        scExistErr = 0;                          /* @00602kish-4053 */
                    }                                            /* @00602kish-4053 */
                    else                                         /* @00602kish-4053 */
                    {                                            /* @00602kish-4053 */
                        uPcExistErr=PC_WAF_NOT_RETURN;           /* @00602kish-4053 */
                    }                                            /* @00602kish-4053 */
                }                                                /* @00602kish-4053 */
                else                                             /* @00602kish-4053 */
                {                                                /* @00602kish-4053 */
				uPcExistErr=PC_WAF_NOT_RETURN;
                }                                                /* @00602kish-4053 */
			}
			else
			{
				LdGetEWafNPos ( ptrUPc, &nowUPos );
				if ( ( nowUPos != SBY ) && ( nowUPos != LID ) )
				{
					nowUPos = HOME;
				}
			}
		}
		else
		{
            if( ( rtn = LdPcVacOff( UPPER, 1 ) ) )           /* @9223kish-01 */
            {                                         /* @8b16kish-01 */
                return( rtn );                        /* @8b16kish-01 */
            }                                         /* @8b16kish-01 */
			LdSetPcWaf ( UPPER, WAF_EMPTY );
            SramSet4( UpcWaf, 0 ) ;                          /* @08910ykor-23437 */
		}
/* ロアピンセット上にウェーハあり ？ */
/* @18b13rame-41195        if( OptionInfoGet( PO_BERNOULLI_ARM ) == 1 )                             @* @06403hyod-16644 */
        if( LdChkBerAirPress_Pc() == 1 )                                         /* @18b13rame-41195 */
        {                                                                        /* @06403hyod-16644 */
            rtn = LdPcRdVac( LOWER, S_ON );                                      /* @06403hyod-16644 */
        }                                                                        /* @06403hyod-16644 */
        else                                                                     /* @06403hyod-16644 */
        {                                                                        /* @06403hyod-16644 */
			dWait  = PcVac [ LOWER ].dWait;
			intV  = PcVac [ LOWER ].intV;
			cTime = PcVac [ LOWER ].cTime;

            if( WarpedLoader == TRUE )                                      /* @12723rame-29272 */
            {                                                               /* @12723rame-29272 */
                rtn = DrvSens( LdPcWtVac, 2, LOWER, S_ON,                   /* @12723rame-29272 */
                               LdPcRdVac, 2, LOWER, S_ON,                   /* @12723rame-29272 */
                               dWait * 2, intV, cTime, 0 );                 /* @12723rame-29272 */
            }                                                               /* @12723rame-29272 */
            else                                                            /* @12723rame-29272 */
            {                                                               /* @12723rame-29272 */
    			/* cTime まで監視する必要なし */
    			rtn = DrvSens( LdPcWtVac , 2 , LOWER , S_ON ,
    			   LdPcRdVac , 2 , LOWER , S_ON , dWait * 2, intV, intV * 3 , 0 ); /* @7718kogi-02 */
            }                                                               /* @12723rame-29272 */
        }                                                                        /* @06403hyod-16644 */
		if( rtn == OK )
		{
#if( P_OCTO )                                                           /* @12723rame-29272 */
            /* バキュームセンサがOFFのピンセットバキュームソレノイド       @12723rame-29272 */
            /* を検出し、そのバキュームをOFFします。                       @12723rame-29272 */
            rtn = LdVacOffPcSol_Off( LOWER );                           /* @12723rame-29272 */
            if( rtn != OK )                                             /* @12723rame-29272 */
            {                                                           /* @12723rame-29272 */
                return( LdSetErr( rtn, 0 ) );                           /* @12723rame-29272 */
            }                                                           /* @12723rame-29272 */
#endif                                                                  /* @12723rame-29272 */
            /* 通常アーム（ベルヌーイ以外）の場合は、サブチャックのウェーハポインタをピンセットに */ /* @12808ykor-29900 */
            /* 移動しているため、移動履歴が残っている場合は以下のシーケンスには入りません         */ /* @12808ykor-29900 */
			if ( LdChkPcWaf (LOWER,WAF_EXIST,&ptrLPc)) 
			{
/* @06527hyod-17006                if(OptionInfoGet(PO_SUBCHUCKZ))                  @* @00602kish-4053 */
                if( ( OptionInfoGet( PO_SUBCHUCKZ ) ) ||         /* @06527hyod-17006 */
                    ( OptionInfoGet( PO_BERNOULLI_WAFBACK) ) )   /* @06527hyod-17006 */
                {                                                /* @00602kish-4053 */
                    if( ptrSc >= 0 )                             /* @00602kish-4053 */
                    {                                            /* @00602kish-4053 */
                        ptrLPc = ptrSc;                          /* @00602kish-4053 */
                        nowLPos = nowSPos;                       /* @00602kish-4053 */
                        ptrSc = -1;                              /* @00602kish-4053 */
                        nowSPos = HOME;                          /* @00602kish-4053 */
                        LdSetPcWaf( LOWER, ptrLPc );             /* @00602kish-4053 */
                    }                                            /* @00602kish-4053 */
                    else if( scExistErr )                        /* @00602kish-4053 */
                    {                                            /* @00602kish-4053 */
                        lPcExistErr=PC_WAF_NOT_RETURN;           /* @00602kish-4053 */
                        scExistErr = 0;                          /* @00602kish-4053 */
                    }                                            /* @00602kish-4053 */
                    else                                         /* @00602kish-4053 */
                    {                                            /* @00602kish-4053 */
                        lPcExistErr=PC_WAF_NOT_RETURN;           /* @00602kish-4053 */
                    }                                            /* @00602kish-4053 */
                }                                                /* @00602kish-4053 */
                else                                             /* @00602kish-4053 */
                {                                                /* @00602kish-4053 */
				lPcExistErr=PC_WAF_NOT_RETURN;
                }                                                /* @00602kish-4053 */
			}
			else
			{
            	LdGetEWafNPos ( ptrLPc, &nowLPos );
            	if ( ( nowLPos != SBY ) && ( nowLPos != LID ) )
            	{
                	nowLPos = HOME;
            	}
        	}
		}
		else
		{
            if( ( rtn = LdPcVacOff( LOWER,1 ) ) )            /* @9223kish-01 */
            {                                         /* @8b16kish-01 */
                return( rtn );                        /* @8b16kish-01 */
            }                                         /* @8b16kish-01 */
			LdSetPcWaf ( LOWER, WAF_EMPTY );
            SramSet4( LpcWaf, 0 ) ;                          /* @08910ykor-23437 */
		}

        if( OptionInfoGet(PO_BERNOULLI_WAFBACK) )           /* @06527hyod-17006 */
        {                                                   /* @06527hyod-17006 */
            if( ( uPcExistErr == PC_WAF_NOT_RETURN ) &&     /* @06527hyod-17006 */
                ( lPcExistErr == PC_WAF_NOT_RETURN ) )      /* @06527hyod-17006 */
            {                                               /* @06527hyod-17006 */
                ptrLPc = ptrUPc;                            /* @06527hyod-17006 */
                nowLPos = nowUPos;                          /* @06527hyod-17006 */
                LdSetPcWaf( LOWER, ptrLPc );                /* @06527hyod-17006 */
                ptrUPc = -1;                                /* @06527hyod-17006 */
                nowUPos = HOME;                             /* @06527hyod-17006 */
            }                                               /* @06527hyod-17006 */
        }                                                   /* @06527hyod-17006 */

/* サブチャックにウェーハあり ？ */
        if( airContSc == 1 )                                        /* @18b13rame-41195 */
        {                                                           /* @18b13rame-41195 */
            rtn = LdScRdVac( S_ON );                                /* @18b13rame-41195 */
        }                                                           /* @18b13rame-41195 */
        else                                                        /* @18b13rame-41195 */
        {                                                           /* @18b13rame-41195 */
    		dWait = ScVac.dWait;
    		intV  = ScVac.intV;
    		cTime = ScVac.cTime;
    		/* cTime まで監視する必要なし */
    		rtn = DrvSens( LdScWtVac , 1 , S_ON , 
    			LdScRdVac , 1 , S_ON , dWait, intV, intV * 3 , 0 );
        }                                                           /* @18b13rame-41195 */
		if( rtn == OK )
		{
			if ( LdChkScWaf ( WAF_EXIST, &ptrSc ) ) 
			{
				scExistErr = SC_WAF_NOT_RETURN;
			}
        	else
        	{  
            	LdGetEWafNPos ( ptrSc, &nowSPos );
            	if ( ( nowSPos != SBY ) && ( nowSPos != LID ) )
            	{
                	nowSPos = HOME;
            	}
        	}
		}
		else
		{
            if( ( rtn = LdScVacOff( 1 ) ) )                  /* @9223kish-01 */
            {                                                /* @9223kish-01 */
                return( rtn );                               /* @9223kish-01 */
            }                                                /* @9223kish-01 */
            if(OptionInfoGet(PO_SUBCHUCKZ))                  /* @00602kish-4053 */
            {                                                /* @00602kish-4053 */
                scExistErr = 0;                              /* @00602kish-4053 */
                if( ( OptionInfoGet(PO_BERNOULLI_WAFBACK) == 0 )  /* @12808ykor-29900 : P-F5 */
                 && ( OptionInfoGet(PO_BERNOULLI_ARM) ) == 0 )    /* @12808ykor-29900 : P-F5 */
                {                                                 /* @12808ykor-29900 : P-F5 */
                    ptrSc = -1;                                   /* @12808ykor-29900 : P-F5 */
                }                                                 /* @12808ykor-29900 : P-F5 */
            }                                                /* @00602kish-4053 */
			LdSetScWaf ( WAF_EMPTY );
            SramSet4( SckWaf, 0 ) ;                          /* @08110ykor-23437 */
		}

#if LD_SPEC_SMIF                                             /* @04218mtog-11614 */
#else                                                        /* @04218mtog-11614 */
        if( LD_INIT_DEBUG == 99 )        /* for debug */     /* @04218mtog-11614 */
        {                                /* for debug */     /* @04218mtog-11614 */
            ptrSc = -1;                  /* for debug */     /* @04218mtog-11614 */
            scExistErr = 0;              /* for debug */     /* @04218mtog-11614 */
        }                                /* for debug */     /* @04218mtog-11614 */

        if( (ptrSc < 0) && (scExistErr == 0) )               /* @04218mtog-11614 */
        {                                                    /* @04218mtog-11614 */
            usePc = -1;                                      /* @04218mtog-11614 */
            if( OK == LdScRdZ(UP) )                          /* @04218mtog-11614 */
            {                                                /* @04218mtog-11614 */
                usePc = UPPER;                               /* @04218mtog-11614 */
            }                                                /* @04218mtog-11614 */

            if( OK == LdScRdZ(MD) )                          /* @04218mtog-11614 */
            {                                                /* @04218mtog-11614 */
                usePc = LOWER;                               /* @04218mtog-11614 */
            }                                                /* @04218mtog-11614 */

            if( usePc != (-1) )                              /* @04218mtog-11614 */
            {                                                /* @04218mtog-11614 */
                if( airContSc == 1 )                                                        /* @18b13rame-41195 */
                {                                                                           /* @18b13rame-41195 */
                    if(( rtn = LdScAirPresCont_OnOff( S_OFF, AIR_PRES_CONT,                 /* @18b13rame-41195 */
                                               PcAirPresDt.PcAirCst[cNo].scPcAir,           /* @18b13rame-41195 */
                                               PcAirPresDt.PcAirCst[cNo].scPcAirTime )))    /* @18b13rame-41195 */
                    {                                                                       /* @18b13rame-41195 */
                        return( rtn );       /* Air圧力制御NG */                            /* @18b13rame-41195 */
                    }                                                                       /* @18b13rame-41195 */
                }                                                                           /* @18b13rame-41195 */
                else                                                                        /* @18b13rame-41195 */
                {                                                                           /* @18b13rame-41195 */
                    if( (rtn = LdScVacOff(1)) )                  /* @04218mtog-11614 */
                    {                                            /* @04218mtog-11614 */
                        return( rtn );                           /* @04218mtog-11614 */
                    }                                            /* @04218mtog-11614 */
                }                                                                             /* @18b13rame-41195 */

                if( (rtn = LdScDn()) )                       /* @04218mtog-11614 */
                {                                            /* @04218mtog-11614 */
                    return( rtn );                           /* @04218mtog-11614 */
                }                                            /* @04218mtog-11614 */

                if( airContBkPc == 1 )                                                      /* @18b13rame-41195 */
                {                                                                           /* @18b13rame-41195 */
                    rtn = LdPcRdVac( usePc, S_ON );                                         /* @18b13rame-41195 */
                    if( rtn == OK )                                                         /* @18b13rame-41195 */
                    {                                                                       /* @18b13rame-41195 */
                        if(( rtn = LdPcAirPresCont_OnOff( usePc, S_ON, AIR_PRES_CONT,       /* @18b13rame-41195 */
                                          PcAirPresDt.PcAirCst[cNo].pcScAir[usePc],         /* @18b13rame-41195 */
                                          PcAirPresDt.PcAirCst[cNo].pcScAirTime[usePc] )))  /* @18b13rame-41195 */
                        {                                                                   /* @18b13rame-41195 */
                            return( rtn );       /* Air圧力制御NG */                        /* @18b13rame-41195 */
                        }                                                                   /* @18b13rame-41195 */
                    }                                                                       /* @18b13rame-41195 */
                }                                                                           /* @18b13rame-41195 */
                else                                                                        /* @18b13rame-41195 */
                {                                                                           /* @18b13rame-41195 */
                    dWait = PcVac[usePc].dWait;                  /* @04218mtog-11614 */
                    intV  = PcVac[usePc].intV;                   /* @04218mtog-11614 */
                    cTime = PcVac[usePc].cTime;                  /* @04218mtog-11614 */

                    if( WarpedLoader == TRUE )                                      /* @12723rame-29272 */
                    {                                                               /* @12723rame-29272 */
                        rtn = DrvSens( LdPcWtVac, 2, usePc, S_ON,                   /* @12723rame-29272 */
                                       LdPcRdVac, 2, usePc, S_ON,                   /* @12723rame-29272 */
                                       dWait, intV, cTime, 0 );                     /* @12723rame-29272 */
                    }                                                               /* @12723rame-29272 */
                    else                                                            /* @12723rame-29272 */
                    {                                                               /* @12723rame-29272 */
                        rtn = DrvSens( LdPcWtVac, 2, usePc, S_ON,    /* @04218mtog-11614 */
                               LdPcRdVac, 2, usePc, S_ON, dWait * 2, /* @04218mtog-11614 */
                                                intV, intV * 3, 0 ); /* @04218mtog-11614 */
                    }                                                               /* @12723rame-29272 */
                }                                                                       /* @18b13rame-41195 */

                if( rtn == OK )
                {                                            /* @04218mtog-11614 */
#if( P_OCTO )                                                                   /* @12723rame-29272 */
                    /* バキュームセンサがOFFのピンセットバキュームソレノイド       @12723rame-29272 */
                    /* を検出し、そのバキュームをOFFします。                       @12723rame-29272 */
                    rtn = LdVacOffPcSol_Off( usePc );                           /* @12723rame-29272 */
                    if( rtn != OK )                                             /* @12723rame-29272 */
                    {                                                           /* @12723rame-29272 */
                        return( LdSetErr( rtn, 0 ) );                           /* @12723rame-29272 */
                    }                                                           /* @12723rame-29272 */
#endif                                                                          /* @12723rame-29272 */
                    ptr = SramGet4( SckWaf );                /* @04218mtog-11614 */
                    if( (ptr != 0x00) && (ptr != 0xff) )     /* @04218mtog-11614 */
                    {                                        /* @04218mtog-11614 */
                        if( usePc == UPPER )                 /* @04218mtog-11614 */
                        {                                    /* @04218mtog-11614 */
                            SramSet4( UpcWaf, ptr );         /* @04218mtog-11614 */
                            uPcExistErr = PC_WAF_NOT_RETURN; /* @04218mtog-11614 */
                        }                                    /* @04218mtog-11614 */

                        if( usePc == LOWER )                 /* @04218mtog-11614 */
                        {                                    /* @04218mtog-11614 */
                            SramSet4( LpcWaf, ptr );         /* @04218mtog-11614 */
                            lPcExistErr = PC_WAF_NOT_RETURN; /* @04218mtog-11614 */
                        }                                    /* @04218mtog-11614 */
                        SramSet4( SckWaf, 0 );               /* @04218mtog-11614 */
                    }                                        /* @04218mtog-11614 */
                    else                                     /* @04218mtog-11614 */
                    {                                        /* @04218mtog-11614 */
                        if( usePc == UPPER )                 /* @04218mtog-11614 */
                        {                                    /* @04218mtog-11614 */
                            uPcExistErr = PC_WAF_NOT_RETURN; /* @04218mtog-11614 */
                        }                                    /* @04218mtog-11614 */

                        if( usePc == LOWER )                 /* @04218mtog-11614 */
                        {                                    /* @04218mtog-11614 */
                            lPcExistErr = PC_WAF_NOT_RETURN; /* @04218mtog-11614 */
                        }                                    /* @04218mtog-11614 */
                    }                                        /* @04218mtog-11614 */
                }                                            /* @04218mtog-11614 */
                else                                         /* @04a22kish-14065 */
                {                                            /* @04a22kish-14065 */
                    if( airContBkPc == 1 )                                                  /* @18b13rame-41195 */
                    {                                                                       /* @18b13rame-41195 */
                        if(( rtn = LdPcAirPresCont_OnOff( usePc, S_OFF, AIR_PRES_CONT,      /* @18b13rame-41195 */
                                          PcAirPresDt.PcAirCst[cNo].pcScAir[usePc],         /* @18b13rame-41195 */
                                          PcAirPresDt.PcAirCst[cNo].pcScAirTime[usePc] )))  /* @18b13rame-41195 */
                        {                                                                   /* @18b13rame-41195 */
                            return( rtn );       /* Air圧力制御NG */                        /* @18b13rame-41195 */
                        }                                                                   /* @18b13rame-41195 */
                    }                                                                       /* @18b13rame-41195 */
                    else                                                                    /* @18b13rame-41195 */
                    {                                                                       /* @18b13rame-41195 */
                        if(( rtn = LdPcVacOff( usePc, 1 )))      /* @04a22kish-14065 */
                        {                                        /* @04a22kish-14065 */
                            return( rtn );                       /* @04a22kish-14065 */
                        }                                        /* @04a22kish-14065 */
                    }                                                                       /* @18b13rame-41195 */
                }                                            /* @04a22kish-14065 */
            }                                                /* @04218mtog-11614 */
        }                                                    /* @04218mtog-11614 */
#endif                                                       /* @04218mtog-11614 */
/********************/
/* ウェーハ返却処理 */
/********************/
		if ( LD_INIT_DEBUG ) printf ( " ウェーハ返却処理開始\n");

/* アッパーピンセットウェーハ返却 */
#if LD_SPEC_SMIF                                          /* @9c02mtog-625 */
        if( SPodExist > 0 )                               /* @9c02mtog-625 */
        {                                                 /* @9c02mtog-625 */
            if ( ptrUPc >= 0 )                            /* @9c02mtog-625 */
            {                                             /* @9c02mtog-625 */
                toId = LdGetWafReturnCst( ptrUPc );       /* @9c02mtog-625 */
                if( toId != ERROR )                       /* @9c02mtog-625 */
                {                                         /* @9c02mtog-625 */
                    rtn = LdWafReturn( nowUPos, toId,     /* @9c02mtog-625 */
                              UPPER, selPaln, ptrUPc );   /* @9c02mtog-625 */
                    wafRetOnInit = 0;                     /* @9c02mtog-625 */
                    if( rtn )                             /* @9c02mtog-625 */
                    {                                     /* @9c02mtog-625 */
                        return( rtn );                    /* @9c02mtog-625 */
                    }                                     /* @9c02mtog-625 */
                }                                         /* @9c02mtog-625 */
                else  /* homeless wafer */                /* @9c02mtog-625 */
                {                                         /* @9c02mtog-625 */
                    LdSetPcWaf( UPPER, ptrUPc );          /* @9c02mtog-625 */
                    LdSetEWafNPos( ptrUPc, HOME );        /* @9c02mtog-625 */
                    rtn = LdWafRtnSmif( UPPER, WTBL_WAF );/* @9c02mtog-625 */
                    wafRetOnInit = 0;                     /* @9c02mtog-625 */
                    if( rtn )                             /* @9c02mtog-625 */
                    {                                     /* @9c02mtog-625 */
                        return( rtn );                    /* @9c02mtog-625 */
                    }                                     /* @9c02mtog-625 */
                }                                         /* @9c02mtog-625 */
            }                                             /* @9c02mtog-625 */
            else  /* wafPtr nothing */                    /* @9c02mtog-625 */
            {                                             /* @9c02mtog-625 */
                if( uPcExistErr )                         /* @9c02mtog-625 */
                {                                         /* @9c02mtog-625 */
                    LdSetPcWaf( UPPER, WTBL_WAF );        /* @9c02mtog-625 */
                    LdSetEWafNPos( WTBL_WAF, HOME );      /* @9c02mtog-625 */
                    rtn = LdWafRtnSmif( UPPER, WTBL_WAF );/* @9c02mtog-625 */
                    wafRetOnInit = 0;                     /* @9c02mtog-625 */
                    if( rtn )                             /* @9c02mtog-625 */
                    {                                     /* @9c02mtog-625 */
                        return( rtn );                    /* @9c02mtog-625 */
                    }                                     /* @9c02mtog-625 */
                }                                         /* @9c02mtog-625 */
            }                                             /* @9c02mtog-625 */

            if ( ptrLPc >= 0 )                            /* @9c02mtog-625 */
            {                                             /* @9c02mtog-625 */
                toId = LdGetWafReturnCst( ptrLPc );       /* @9c02mtog-625 */
                if( toId != ERROR )                       /* @9c02mtog-625 */
                {                                         /* @9c02mtog-625 */
                    rtn = LdWafReturn( nowUPos, toId,     /* @9c02mtog-625 */
                              LOWER, selPaln, ptrLPc );   /* @9c02mtog-625 */
                    wafRetOnInit = 0;                     /* @9c02mtog-625 */
                    if( rtn )                             /* @9c02mtog-625 */
                    {                                     /* @9c02mtog-625 */
                        return( rtn );                    /* @9c02mtog-625 */
                    }                                     /* @9c02mtog-625 */
                }                                         /* @9c02mtog-625 */
                else  /* homeless wafer */                /* @9c02mtog-625 */
                {                                         /* @9c02mtog-625 */
                    LdSetPcWaf( LOWER, ptrLPc );          /* @9c02mtog-625 */
                    LdSetEWafNPos( ptrLPc, HOME );        /* @9c02mtog-625 */
                    rtn = LdWafRtnSmif( LOWER, WTBL_WAF );/* @9c02mtog-625 */
                    wafRetOnInit = 0;                     /* @9c02mtog-625 */
                    if( rtn )                             /* @9c02mtog-625 */
                    {                                     /* @9c02mtog-625 */
                        return( rtn );                    /* @9c02mtog-625 */
                    }                                     /* @9c02mtog-625 */
                }                                         /* @9c02mtog-625 */
            }                                             /* @9c02mtog-625 */
            else  /* wafPtr nothing */                    /* @9c02mtog-625 */
            {                                             /* @9c02mtog-625 */
                if( lPcExistErr )                         /* @9c02mtog-625 */
                {                                         /* @9c02mtog-625 */
                    LdSetPcWaf( LOWER, WTBL_WAF );        /* @9c02mtog-625 */
                    LdSetEWafNPos( WTBL_WAF, HOME );      /* @9c02mtog-625 */
                    rtn = LdWafRtnSmif( LOWER, WTBL_WAF );/* @9c02mtog-625 */
                    wafRetOnInit = 0;                     /* @9c02mtog-625 */
                    if( rtn )                             /* @9c02mtog-625 */
                    {                                     /* @9c02mtog-625 */
                        return( rtn );                    /* @9c02mtog-625 */
                    }                                     /* @9c02mtog-625 */
                }                                         /* @9c02mtog-625 */
            }                                             /* @9c02mtog-625 */
            if ( ptrSc >= 0 )                             /* @9c02mtog-625 */
            {                                             /* @9c02mtog-625 */
                if( LdScRdZ(DN) == OK )                   /* @9c02mtog-625 */
                {                                         /* @9c02mtog-625 */
                    return( LdSetErr(SC_POS_ILLEGAL,0) ); /* @9c02mtog-625 */
                }                                         /* @9c02mtog-625 */
                if( LdScRdZ(MD) == OK )                   /* @9c02mtog-625 */
                {                                         /* @9c02mtog-625 */
                    usePc = LOWER;                        /* @9c02mtog-625 */
                    home = LdGetStep( MTR_LPC,            /* @9c02mtog-625 */
                                 PcPosDt.pcHm[usePc] );   /* @9c02mtog-625 */
/* @01b20kish-7382                    if( rtn = LdSubSc2Pc(usePc, home) )   @* @9c02mtog-625 */
                    if(( rtn = LdSubSc2Pc(usePc, home) )) /* @01b20kish-7382 */
                    {                                     /* @9c02mtog-625 */
                        return( rtn );                    /* @9c02mtog-625 */
                    }                                     /* @9c02mtog-625 */
                }                                         /* @9c02mtog-625 */
                if( LdScRdZ(UP) == OK )                   /* @9c02mtog-625 */
                {                                         /* @9c02mtog-625 */
                    usePc = UPPER;                        /* @9c02mtog-625 */
                    home = LdGetStep( MTR_UPC,            /* @9c02mtog-625 */
                                  PcPosDt.pcHm[usePc] );  /* @9c02mtog-625 */
/* @01b20kish-7382                    if( rtn = LdSubSc2Pc(usePc, home) )   @* @9c02mtog-625 */
                    if(( rtn = LdSubSc2Pc(usePc, home) )) /* @01b20kish-7382 */
                    {                                     /* @9c02mtog-625 */
                        return( rtn );                    /* @9c02mtog-625 */
                    }                                     /* @9c02mtog-625 */
                }                                         /* @9c02mtog-625 */
                toId = LdGetWafReturnCst( ptrSc );        /* @9c02mtog-625 */
                if( toId != ERROR )                       /* @9c02mtog-625 */
                {                                         /* @9c02mtog-625 */
                    rtn = LdWafReturn( HOME, toId, usePc, /* @9c02mtog-625 */
                                        selPaln, ptrSc ); /* @9c02mtog-625 */
                    wafRetOnInit = 0;                     /* @9c02mtog-625 */
                    if( rtn )                             /* @9c02mtog-625 */
                    {                                     /* @9c02mtog-625 */
                        return( rtn );                    /* @9c02mtog-625 */
                    }                                     /* @9c02mtog-625 */
                }                                         /* @9c02mtog-625 */
                else                                      /* @9c02mtog-625 */
                {                                         /* @9c02mtog-625 */
                    LdSetPcWaf( usePc, ptrSc );           /* @9c02mtog-625 */
                    LdSetEWafNPos( ptrSc, HOME );         /* @9c02mtog-625 */
                    rtn = LdWafRtnSmif( usePc, WTBL_WAF );/* @9c02mtog-625 */
                    wafRetOnInit = 0;                     /* @9c02mtog-625 */
                    if( rtn )                             /* @9c02mtog-625 */
                    {                                     /* @9c02mtog-625 */
                        return( rtn );                    /* @9c02mtog-625 */
                    }                                     /* @9c02mtog-625 */
                }                                         /* @9c02mtog-625 */
            }                                             /* @9c02mtog-625 */
            else  /* wafPtr nothing */                    /* @9c02mtog-625 */
            {                                             /* @9c02mtog-625 */
                if( scExistErr )                          /* @9c02mtog-625 */
                {                                         /* @9c02mtog-625 */
                    if( LdScRdZ(DN) == OK )               /* @9c02mtog-625 */
                    {                                     /* @9c02mtog-625 */
                        return( LdSetErr(SC_POS_ILLEGAL,0) );/* @9c02mtog-625 */
                    }                                     /* @9c02mtog-625 */
                    if( LdScRdZ(MD) == OK )               /* @9c02mtog-625 */
                    {                                     /* @9c02mtog-625 */
                        usePc = LOWER;                    /* @9c02mtog-625 */
                    }                                     /* @9c02mtog-625 */
                    if( LdScRdZ(UP) == OK )               /* @9c02mtog-625 */
                    {                                     /* @9c02mtog-625 */
                        usePc = UPPER;                    /* @9c02mtog-625 */
                    }                                     /* @9c02mtog-625 */
                    LdPcWtVac( usePc, 1 );                /* @9c02mtog-625 */
/* @01b20kish-7382                    if( rtn = LdScVacOff(1) )             @* @9c02mtog-625 */
                    if(( rtn = LdScVacOff(1) ))           /* @01b20kish-7382 */
                    {                                     /* @9c02mtog-625 */
                        return( rtn );                    /* @9c02mtog-625 */
                    }                                     /* @9c02mtog-625 */
/* @01b20kish-7382                    if( rtn = LdScDn() )                  @* @9c02mtog-625 */
                    if(( rtn = LdScDn() ))                /* @01b20kish-7382 */
                    {                                     /* @9c02mtog-625 */
                        return( rtn );                    /* @9c02mtog-625 */
                    }                                     /* @9c02mtog-625 */
/* @01b20kish-7382                    if( rtn = LdPcVacOn( usePc, 0 ) )     @* @9c02mtog-625 */
                    if(( rtn = LdPcVacOn( usePc, 0 ) ))   /* @01b20kish-7382 */
                    {                                     /* @9c02mtog-625 */
                        return( rtn );                    /* @9c02mtog-625 */
                    }                                     /* @9c02mtog-625 */
                    LdSetPcWaf( usePc, WTBL_WAF );        /* @9c02mtog-625 */
                    LdSetEWafNPos( WTBL_WAF, HOME );      /* @9c02mtog-625 */
                    rtn = LdWafRtnSmif( usePc, WTBL_WAF );/* @9c02mtog-625 */
                    wafRetOnInit = 0;                     /* @9c02mtog-625 */
                    if( rtn )                             /* @9c02mtog-625 */
                    {                                     /* @9c02mtog-625 */
                        return( rtn );                    /* @9c02mtog-625 */
                    }                                     /* @9c02mtog-625 */
                }                                         /* @9c02mtog-625 */
            }                                             /* @9c02mtog-625 */
        }                                                 /* @9c02mtog-625 */
        else                                              /* @9c02mtog-625 */
        {                                                 /* @9c02mtog-625 */
            if( (ptrUPc >= 0) || (ptrLPc >= 0) ||         /* @9c02mtog-625 */
                    (ptrSc >= 0)  || (uPcExistErr) ||     /* @9c02mtog-625 */
                        (lPcExistErr) || (scExistErr) )   /* @9c02mtog-625 */
            {                                             /* @9c02mtog-625 */
                if( (MtrRdStatus(MTR_UPC, 2) &            /* @9c02mtog-625 */
                                  MTR2_ORG_BIT) == 0 )    /* @9c02mtog-625 */
                {                                         /* @9c02mtog-625 */
                    return( (LdSetErr(IDX_UPC_NOT_HOME,   /* @9c02mtog-625 */
                                                  0)) );  /* @9c02mtog-625 */
                }                                         /* @9c02mtog-625 */
                if( (MtrRdStatus(MTR_LPC, 2) &            /* @9c02mtog-625 */
                                  MTR2_ORG_BIT) == 0 )    /* @9c02mtog-625 */
                {                                         /* @9c02mtog-625 */
                    return( (LdSetErr(IDX_LPC_NOT_HOME,   /* @9c02mtog-625 */
                                                  0)) );  /* @9c02mtog-625 */
                }                                         /* @9c02mtog-625 */
                LdPartDt.idxPos = 0;                      /* @9c02mtog-625 */
                memset( &rcvBufSmf[0], 0x00,              /* @9c02mtog-625 */
                                sizeof(rcvBufSmf) );      /* @9c02mtog-625 */
                memset( &sndBufSmf[0], 0x00,              /* @9c02mtog-625 */
                                sizeof(sndBufSmf) );      /* @9c02mtog-625 */
                strcpy( &sndBufSmf[0], "LIXMVHM" );       /* @9c02mtog-625 */
                LdMsgSndSmif( &sndBufSmf[0], 2 );         /* @9c02mtog-625 */
                LdPartDt.idxPos = IDX_HM;                 /* @9c02mtog-625 */
            }                                             /* @9c02mtog-625 */
        }                                                 /* @9c02mtog-625 */
#else                                                     /* @9c02mtog-625 */
        ptr = SramGet4( UpcWaf) ;                           /* @6c13mtog-17805 */
        if( (ptr != 0x00) && (ptr != 0xff) &&               /* @6c13mtog-17805 */
            (ptr != (WTBL_WAF+1)) && (ptr != (BTBL_WAF+1)) )/* @6c13mtog-17805 */
        {                                                   /* @6c13mtog-17805 */
            if( ptr < SYS_SLOT_MAX )                        /* @6c13mtog-17805 */
            {                                               /* @6c13mtog-17805 */
                cNo = 0;                                    /* @6c13mtog-17805 */
            }                                               /* @6c13mtog-17805 */
            if( ptr >= SYS_SLOT_MAX )                       /* @6c13mtog-17805 */
            {                                               /* @6c13mtog-17805 */
                cNo = 1;                                    /* @6c13mtog-17805 */
            }                                               /* @6c13mtog-17805 */
            if( OptionInfoGet(PO_2ND1CST) )                 /* @6c13mtog-17805 */
            {                                               /* @6c13mtog-17805 */
                cNo = 1;                                    /* @6c13mtog-17805 */
            }                                               /* @6c13mtog-17805 */
        }                                                   /* @6c13mtog-17805 */

		if ( ptrUPc >= 0 )
		{
            toId = LdGetWafReturnCst( ptrUPc );  /* @7a08mtog-00 */
            if( toId == ERROR )                  /* @7a08mtog-00 */
            {                                    /* @7a08mtog-00 */
    			toId = LdGetWafReturn ( ptrUPc );	
/* @06c13mtog-17805    			if ( toId == ERROR ) toId = 0; */
                if( toId == ERROR )                         /* @6c13mtog-17805 */
                {                                           /* @6c13mtog-17805 */
                    if( (ptr == 0x00) || (ptr == 0xff) ||   /* @6c13mtog-17805 */
                        (ptr == (WTBL_WAF+1)) ||            /* @6c13mtog-17805 */
                        (ptr == (BTBL_WAF+1)) )             /* @6c13mtog-17805 */
                    {                                       /* @6c13mtog-17805 */
                        if( ptr == (BTBL_WAF+1) )           /* @6c13mtog-17805 */
                        {                                   /* @6c13mtog-17805 */
                            toId = B_TBL;                   /* @6c13mtog-17805 */
                        }                                   /* @6c13mtog-17805 */
                        else                                /* @6c13mtog-17805 */
                        {                                   /* @6c13mtog-17805 */
                            toId = 0;                       /* @6c13mtog-17805 */
                        }                                   /* @6c13mtog-17805 */
                    }                                       /* @6c13mtog-17805 */
                    else                                    /* @6c13mtog-17805 */
                    {                                       /* @6c13mtog-17805 */
                        ptr --;                             /* @6c13mtog-17805 */
                        toId = LdMapWafReturn( ptr, cNo );  /* @6c13mtog-17805 */
                        if( toId != 0 )                     /* @6c13mtog-17805 */
                        {                                   /* @6c13mtog-17805 */
                            ptrUPc = ptr;                   /* @6c13mtog-17805 */
                        }                                   /* @6c13mtog-17805 */
                    }                                       /* @6c13mtog-17805 */
                }                                           /* @6c13mtog-17805 */
            }                                    /* @7a08mtog-00 */
            rtn = LdWafReturn ( nowUPos, toId, UPPER, selPaln, ptrUPc );
            wafRetOnInit = 0;  /* @7805mtog-00 */
            if ( rtn ) return ( rtn );
		}
		else
		{
			if ( uPcExistErr )
			{
/* @06c13mtog-17805				toId = 0; */
/* @06c13mtog-17805				LdSetPcWaf ( UPPER, WTBL_WAF ); */
/* @06c13mtog-17805				LdSetEWafNPos ( WTBL_WAF, HOME ); */
/* @06c13mtog-17805				rtn = LdWafReturn ( hOME, toId, UPPER, selPaln, WTBL_WAF ); */
/* @06c13mtog-17805                wafRetOnInit = 0;  @* @7805mtog-00 *@ */
/* @06c13mtog-17805				if ( rtn ) return ( rtn ); */

                ptr = SramGet4( UpcWaf ) ;                  /* @06c13mtog-17805 */
                usePc = UPPER;                              /* @06c13mtog-17805 */
                if( (ptr != 0x00) && (ptr != 0xff) &&       /* @06c13mtog-17805 */
                    (ptr != (WTBL_WAF+1))          &&       /* @06c13mtog-17805 */
                    (ptr != (BTBL_WAF+1)) )                 /* @06c13mtog-17805 */
                {                                           /* @06c13mtog-17805 */
                    ptr --;                                 /* @06c13mtog-17805 */
                    toId = LdMapWafReturn( ptr, cNo );      /* @06c13mtog-17805 */
                    if( toId == 0 )                         /* @06c13mtog-17805 */
                    {                                       /* @06c13mtog-17805 */
                        LdSetPcWaf( UPPER, WTBL_WAF );      /* @06c13mtog-17805 */
                        LdSetEWafNPos( WTBL_WAF, HOME );    /* @06c13mtog-17805 */
                        ptr = WTBL_WAF;                     /* @06c13mtog-17805 */
                    }                                       /* @06c13mtog-17805 */
                    else                                    /* @06c13mtog-17805 */
                    {                                       /* @06c13mtog-17805 */
                        LdSetPcWaf( UPPER, ptr );           /* @06c13mtog-17805 */
                        LdSetEWafNPos( ptr, HOME );         /* @06c13mtog-17805 */
                    }                                       /* @06c13mtog-17805 */
                }                                           /* @06c13mtog-17805 */
                else if( ptr == (BTBL_WAF+1) )              /* @06c13mtog-17805 */
                {                                           /* @06c13mtog-17805 */
                    ptr -- ;                                /* @06c13mtog-17805 */
                    toId = B_TBL;                           /* @06c13mtog-17805 */
                    LdSetPcWaf( UPPER, ptr );               /* @06c13mtog-17805 */
                    LdSetEWafNPos( ptr, HOME );             /* @06c13mtog-17805 */
                }                                           /* @06c13mtog-17805 */
                else                                        /* @06c13mtog-17805 */
                {                                           /* @06c13mtog-17805 */
                    toId = 0;                               /* @06c13mtog-17805 */
                    LdSetPcWaf( UPPER, WTBL_WAF );          /* @06c13mtog-17805 */
                    LdSetEWafNPos( WTBL_WAF, HOME );        /* @06c13mtog-17805 */
                    ptr = WTBL_WAF;                         /* @06c13mtog-17805 */
                }                                           /* @06c13mtog-17805 */

                rtn = LdWafReturn( HOME, toId, usePc,       /* @06c13mtog-17805 */
                                           selPaln, ptr );  /* @06c13mtog-17805 */
                wafRetOnInit = 0;                           /* @06c13mtog-17805 */
                if( rtn )                                   /* @06c13mtog-17805 */
                {                                           /* @06c13mtog-17805 */
                    return( rtn );                          /* @06c13mtog-17805 */
                }                                           /* @06c13mtog-17805 */
			}
		}

/* ロアピンセットウェーハ返却 */
        ptr = SramGet4( LpcWaf ) ;                          /* @06c13mtog-17805 */
        if( (ptr != 0x00) && (ptr != 0xff) &&               /* @06c13mtog-17805 */
            (ptr != (WTBL_WAF+1)) && (ptr != (BTBL_WAF+1)) )/* @06c13mtog-17805 */
        {                                                   /* @06c13mtog-17805 */
            if( ptr < SYS_SLOT_MAX )                        /* @06c13mtog-17805 */
            {                                               /* @06c13mtog-17805 */
                cNo = 0;                                    /* @06c13mtog-17805 */
            }                                               /* @06c13mtog-17805 */
            if( ptr >= SYS_SLOT_MAX )                       /* @06c13mtog-17805 */
            {                                               /* @06c13mtog-17805 */
                cNo = 1;                                    /* @06c13mtog-17805 */
            }                                               /* @06c13mtog-17805 */
            if( OptionInfoGet(PO_2ND1CST) )                 /* @06c13mtog-17805 */
            {                                               /* @06c13mtog-17805 */
                cNo = 1;                                    /* @06c13mtog-17805 */
            }                                               /* @06c13mtog-17805 */
        }                                                   /* @06c13mtog-17805 */

		if ( ptrLPc >= 0 )
		{
            toId = LdGetWafReturnCst( ptrLPc );  /* @7a08mtog-00 */
            if( toId == ERROR )                  /* @7a08mtog-00 */
            {                                    /* @7a08mtog-00 */
			    toId = LdGetWafReturn ( ptrLPc );	
/* @06c13mtog-17805			    if ( toId == ERROR ) toId = 0; */
                if( toId == ERROR )                         /* @06c13mtog-17805 */
                {                                           /* @06c13mtog-17805 */
                    if( (ptr == 0x00) || (ptr == 0xff) ||   /* @06c13mtog-17805 */
                        (ptr == (WTBL_WAF+1)) ||            /* @06c13mtog-17805 */
                        (ptr == (BTBL_WAF+1)) )             /* @06c13mtog-17805 */
                    {                                       /* @06c13mtog-17805 */
                        if( ptr == (BTBL_WAF+1) )           /* @06c13mtog-17805 */
                        {                                   /* @06c13mtog-17805 */
                            toId = B_TBL;                   /* @06c13mtog-17805 */
                        }                                   /* @06c13mtog-17805 */
                        else                                /* @06c13mtog-17805 */
                        {                                   /* @06c13mtog-17805 */
                            toId = 0;                       /* @06c13mtog-17805 */
                        }                                   /* @06c13mtog-17805 */
                    }                                       /* @06c13mtog-17805 */
                    else                                    /* @06c13mtog-17805 */
                    {                                       /* @06c13mtog-17805 */
                        ptr --;                             /* @06c13mtog-17805 */
                        toId = LdMapWafReturn( ptr, cNo );  /* @06c13mtog-17805 */
                        {                                   /* @06c13mtog-17805 */
                            ptrLPc = ptr;                   /* @06c13mtog-17805 */
                        }                                   /* @06c13mtog-17805 */
                    }                                       /* @06c13mtog-17805 */
                }                                           /* @06c13mtog-17805 */
            }                                    /* @7a08mtog-00 */
			rtn = LdWafReturn ( nowLPos, toId, LOWER, selPaln, ptrLPc );
            wafRetOnInit = 0;  /* @7805mtog-00 */
			if ( rtn ) return ( rtn );
		}
		else
		{
			if ( lPcExistErr )
			{
/* @06c13mtog-17805				toId = 0; */
/* @06c13mtog-17805				LdSetPcWaf ( LOWER, WTBL_WAF ); */
/* @06c13mtog-17805				LdSetEWafNPos ( WTBL_WAF, HOME ); */
/* @06c13mtog-17805				rtn = LdWafReturn ( HOME, toId, LOWER, selPaln, WTBL_WAF ); */
/* @06c13mtog-17805                wafRetOnInit = 0;  @* @7805mtog-00 *@ */
/* @06c13mtog-17805				if ( rtn ) return ( rtn ); */

                ptr = SramGet4( LpcWaf ) ;                  /* @06c13mtog-17805 */
                usePc = LOWER;                              /* @06c13mtog-17805 */
                if( (ptr != 0x00) && (ptr != 0xff) &&       /* @06c13mtog-17805 */
                    (ptr != (WTBL_WAF+1))          &&       /* @06c13mtog-17805 */
                    (ptr != (BTBL_WAF+1)) )                 /* @06c13mtog-17805 */
                {                                           /* @06c13mtog-17805 */
                    ptr --;                                 /* @06c13mtog-17805 */
                    toId = LdMapWafReturn( ptr, cNo );      /* @06c13mtog-17805 */
                    if( toId == 0 )                         /* @06c13mtog-17805 */
                    {                                       /* @06c13mtog-17805 */
                        LdSetPcWaf ( LOWER, WTBL_WAF );     /* @06c13mtog-17805 */
                        LdSetEWafNPos ( WTBL_WAF, HOME );   /* @06c13mtog-17805 */
                        ptr = WTBL_WAF;                     /* @06c13mtog-17805 */
                    }                                       /* @06c13mtog-17805 */
                    else                                    /* @06c13mtog-17805 */
                    {                                       /* @06c13mtog-17805 */
                        LdSetPcWaf( LOWER, ptr );           /* @06c13mtog-17805 */
                        LdSetEWafNPos( ptr, HOME );         /* @06c13mtog-17805 */
                    }                                       /* @06c13mtog-17805 */
                }                                           /* @06c13mtog-17805 */
                else if( ptr == (BTBL_WAF+1) )              /* @06c13mtog-17805 */
                {                                           /* @06c13mtog-17805 */
                    ptr -- ;                                /* @06c13mtog-17805 */
                    toId = B_TBL;                           /* @06c13mtog-17805 */
                    LdSetPcWaf( LOWER, ptr );               /* @06c13mtog-17805 */
                    LdSetEWafNPos( ptr, HOME );             /* @06c13mtog-17805 */
                }                                           /* @06c13mtog-17805 */
                else                                        /* @06c13mtog-17805 */
                {                                           /* @06c13mtog-17805 */
                    toId = 0;                               /* @06c13mtog-17805 */
                    LdSetPcWaf( LOWER, WTBL_WAF );          /* @06c13mtog-17805 */
                    LdSetEWafNPos( WTBL_WAF, HOME );        /* @06c13mtog-17805 */
                    ptr = WTBL_WAF;                         /* @06c13mtog-17805 */
                }                                           /* @06c13mtog-17805 */
                rtn = LdWafReturn( HOME, toId, usePc,       /* @06c13mtog-17805 */
                                           selPaln, ptr );  /* @06c13mtog-17805 */
                wafRetOnInit = 0;                           /* @06c13mtog-17805 */
                if( rtn )                                   /* @06c13mtog-17805 */
                {                                           /* @06c13mtog-17805 */
                    return( rtn );                          /* @06c13mtog-17805 */
                }                                           /* @06c13mtog-17805 */
			}
		}

/* サブチャックウェーハ返却 */
        ptr = SramGet4( SckWaf ) ;                          /* @06c13mtog-17805 */
        if( (ptr != 0x00) && (ptr != 0xff) &&               /* @06c13mtog-17805 */
            (ptr != (WTBL_WAF+1)) && (ptr != (BTBL_WAF+1)) )/* @06c13mtog-17805 */
        {                                                   /* @06c13mtog-17805 */
            if( ptr < SYS_SLOT_MAX )                        /* @06c13mtog-17805 */
            {                                               /* @06c13mtog-17805 */
                cNo = 0;                                    /* @06c13mtog-17805 */
            }                                               /* @06c13mtog-17805 */
            if( ptr >= SYS_SLOT_MAX )                       /* @06c13mtog-17805 */
            {                                               /* @06c13mtog-17805 */
                cNo = 1;                                    /* @06c13mtog-17805 */
            }                                               /* @06c13mtog-17805 */
            if( OptionInfoGet(PO_2ND1CST) )                 /* @06c13mtog-17805 */
            {                                               /* @06c13mtog-17805 */
                cNo = 1;                                    /* @06c13mtog-17805 */
            }                                               /* @06c13mtog-17805 */
        }                                                   /* @06c13mtog-17805 */

		if ( ptrSc >= 0 )
		{
            toId = LdGetWafReturnCst( ptrSc );   /* @7a08mtog-00 */
            if( toId == ERROR )                  /* @7a08mtog-00 */
            {                                    /* @7a08mtog-00 */
			    toId = LdGetWafReturn ( ptrSc );	
/* @06c13mtog-17805			    if ( toId == ERROR ) toId = 0; */
                if( toId == ERROR )                         /* @06c13mtog-17805 */
                {                                           /* @06c13mtog-17805 */
                    if( (ptr == 0x00) || (ptr == 0xff) ||   /* @06c13mtog-17805 */
                        (ptr == (WTBL_WAF+1)) ||            /* @06c13mtog-17805 */
                        (ptr == (BTBL_WAF+1)) )             /* @06c13mtog-17805 */
                    {                                       /* @06c13mtog-17805 */
                        if( ptr == (BTBL_WAF+1) )           /* @06c13mtog-17805 */
                        {                                   /* @06c13mtog-17805 */
                            toId = B_TBL;                   /* @06c13mtog-17805 */
                        }                                   /* @06c13mtog-17805 */
                        else                                /* @06c13mtog-17805 */
                        {                                   /* @06c13mtog-17805 */
                            toId = 0;                       /* @06c13mtog-17805 */
                        }                                   /* @06c13mtog-17805 */
                    }                                       /* @06c13mtog-17805 */
                    else                                    /* @06c13mtog-17805 */
                    {                                       /* @06c13mtog-17805 */
                        ptr --;                             /* @06c13mtog-17805 */
                        toId = LdMapWafReturn( ptr, cNo );  /* @06c13mtog-17805 */
                        if( toId != 0 )                     /* @06c13mtog-17805 */
                        {                                   /* @06c13mtog-17805 */
                            ptrSc = ptr;                    /* @06c13mtog-17805 */
                        }                                   /* @06c13mtog-17805 */
                    }                                       /* @06c13mtog-17805 */
                }                                           /* @06c13mtog-17805 */
            }                                    /* @7a08mtog-00 */

            if( OptionInfoGet(PO_BERNOULLI_ARM) == 1 )                       /* @06323mtog-16644 */
            {                                                                /* @06323mtog-16644 */
                if( LdScRdZ(DN) == OK )                                      /* @06323mtog-16644 */
                {                                                            /* @06323mtog-16644 */
				    usePc = LOWER;                                           /* @06323mtog-16644 */
    				home = LdGetStep( MTR_LPC, PcPosDt.pcHm[usePc] );        /* @06323mtog-16644 */
                    if( (rtn = LdSubSc2Pc(usePc, home)) )                    /* @06323mtog-16644 */
                    {                                                        /* @06323mtog-16644 */
                        return( rtn );                                       /* @06323mtog-16644 */
                    }                                                        /* @06323mtog-16644 */
                }                                                            /* @06323mtog-16644 */
            }                                                                /* @06323mtog-16644 */
            else                                                             /* @06323mtog-16644 */
            {                                                                /* @06323mtog-16644 */
				if ( LdScRdZ ( DN ) == OK ) return ( LdSetErr ( SC_POS_ILLEGAL,0 ) );
            }                                                                /* @06323mtog-16644 */

			if ( LdScRdZ ( MD ) == OK ) 
			{
				usePc = LOWER;
				home = LdGetStep ( MTR_LPC, PcPosDt.pcHm [ usePc ] );
                if( ( rtn = LdSubSc2Pc( usePc, home ) ) )    /* @9223kish-01 */
								return ( rtn );
			}
			if ( LdScRdZ ( UP ) == OK ) 
			{
				usePc = UPPER;
				home = LdGetStep ( MTR_UPC, PcPosDt.pcHm [ usePc ] );
                if( ( rtn = LdSubSc2Pc( usePc, home ) ) )    /* @9223kish-01 */
								return ( rtn );
			}

			rtn = LdWafReturn ( HOME, toId, usePc, selPaln, ptrSc );
            wafRetOnInit = 0;  /* @7805mtog-00 */
			if ( rtn ) return ( rtn );
		}
		else
		{
			if ( scExistErr )
			{
/* @06c13mtog-17805				toId = 0; */
                if( OptionInfoGet( PO_BERNOULLI_ARM ) == 0 )                     /* @06403hyod-16644 */
                {                                                                /* @06403hyod-16644 */
	    			if ( LdScRdZ ( DN ) == OK ) return ( LdSetErr ( SC_POS_ILLEGAL,0));
                }                                                                /* @06403hyod-16644 */
				if ( LdScRdZ ( MD ) == OK ) 
				{
					usePc = LOWER;
				}
				if ( LdScRdZ ( UP ) == OK ) 
				{
					usePc = UPPER;
				}

                if( airContBkPc == 1 )                                                  /* @18b13rame-41195 */
                {                                                                       /* @18b13rame-41195 */
                    if(( rtn = LdPcAirPresCont_OnOff( usePc, S_ON, AIR_PRES_CONT,       /* @18b13rame-41195 */
                                      PcAirPresDt.PcAirCst[cNo].pcScAir[usePc],         /* @18b13rame-41195 */
                                      PcAirPresDt.PcAirCst[cNo].pcScAirTime[usePc] )))  /* @18b13rame-41195 */
                    {                                                                   /* @18b13rame-41195 */
                        return( rtn );       /* Air圧力制御NG */                        /* @18b13rame-41195 */
                    }                                                                   /* @18b13rame-41195 */
                }                                                                       /* @18b13rame-41195 */
                else                                                                    /* @18b13rame-41195 */
                {                                                                       /* @18b13rame-41195 */
    				LdPcWtVac ( usePc, 1 );
                }                                                                       /* @18b13rame-41195 */

                if( airContSc == 1 )                                                    /* @18b13rame-41195 */
                {                                                                       /* @18b13rame-41195 */
                    if(( rtn = LdScAirPresCont_OnOff( S_OFF, AIR_PRES_CONT,             /* @18b13rame-41195 */
                                      PcAirPresDt.PcAirCst[cNo].scPcAir,                /* @18b13rame-41195 */
                                      PcAirPresDt.PcAirCst[cNo].scPcAirTime )))         /* @18b13rame-41195 */
                    {                                                                   /* @18b13rame-41195 */
                        return( rtn );       /* Air圧力制御NG */                        /* @18b13rame-41195 */
                    }                                                                   /* @18b13rame-41195 */
                }                                                                       /* @18b13rame-41195 */
                else                                                                    /* @18b13rame-41195 */
                {                                                                       /* @18b13rame-41195 */
                    if( ( rtn = LdScVacOff( 1 ) ) )              /* @9223kish-01 */
                    {                                            /* @9223kish-01 */
                        return( rtn );                           /* @9223kish-01 */
                    }                                            /* @9223kish-01 */
                }                                                                       /* @18b13rame-41195 */

                if( ( rtn = LdScDn() ) )                     /* @9223kish-01 */
                {                                            /* @9223kish-01 */
                    return( rtn );                           /* @9223kish-01 */
                }                                            /* @9223kish-01 */

                if( ( rtn = LdPcVacOn( usePc, 0 ) ) )        /* @9223kish-01 */
                {                                            /* @9223kish-01 */
                    return( rtn );                           /* @9223kish-01 */
                }                                            /* @9223kish-01 */
/* @06c13mtog-17805				LdSetPcWaf ( usePc, WTBL_WAF ); */
/* @06c13mtog-17805				LdSetEWafNPos ( WTBL_WAF, HOME ); */
/* @06c13mtog-17805				rtn = LdWafReturn ( HOME, toId, usePc, selPaln, WTBL_WAF ); */
/* @06c13mtog-17805                wafRetOnInit = 0;  @* @7805mtog-00 *@ */
/* @06c13mtog-17805				if ( rtn ) return ( rtn ); */

                ptr = SramGet4( SckWaf ) ;                  /* @06c13mtog-17805 */
                if( (ptr != 0x00) && (ptr != 0xff) &&       /* @06c13mtog-17805 */
                    (ptr != (WTBL_WAF+1))          &&       /* @06c13mtog-17805 */
                    (ptr != (BTBL_WAF+1)) )                 /* @06c13mtog-17805 */
                {                                           /* @06c13mtog-17805 */
                    ptr --;                                 /* @06c13mtog-17805 */
                    toId = LdMapWafReturn( ptr, cNo );      /* @06c13mtog-17805 */
                    if( toId == 0 )                         /* @06c13mtog-17805 */
                    {                                       /* @06c13mtog-17805 */
                        LdSetPcWaf( usePc, WTBL_WAF );      /* @06c13mtog-17805 */
                        LdSetEWafNPos( WTBL_WAF, HOME );    /* @06c13mtog-17805 */
                        ptr = WTBL_WAF;                     /* @06c13mtog-17805 */
                    }                                       /* @06c13mtog-17805 */
                    else                                    /* @06c13mtog-17805 */
                    {                                       /* @06c13mtog-17805 */
                        LdSetPcWaf( usePc, ptr );           /* @06c13mtog-17805 */
                        LdSetEWafNPos( ptr, HOME );         /* @06c13mtog-17805 */
                    }                                       /* @06c13mtog-17805 */
                }                                           /* @06c13mtog-17805 */
                else if( ptr == (BTBL_WAF+1) )              /* @06c13mtog-17805 */
                {                                           /* @06c13mtog-17805 */
                    ptr -- ;                                /* @06c13mtog-17805 */
                    toId = B_TBL;                           /* @06c13mtog-17805 */
                    LdSetPcWaf( usePc, ptr );               /* @06c13mtog-17805 */
                    LdSetEWafNPos( ptr, HOME );             /* @06c13mtog-17805 */
                }                                           /* @06c13mtog-17805 */
                else                                        /* @06c13mtog-17805 */
                {                                           /* @06c13mtog-17805 */
                    toId = 0;                               /* @06c13mtog-17805 */
                    LdSetPcWaf( usePc, WTBL_WAF );          /* @06c13mtog-17805 */
                    LdSetEWafNPos( WTBL_WAF, HOME );        /* @06c13mtog-17805 */
                    ptr = WTBL_WAF;                         /* @06c13mtog-17805 */
                }                                           /* @06c13mtog-17805 */

                SramSet4( SckWaf, 0 ) ;                     /* @06c13mtog-17805 */

				rtn = LdWafReturn( HOME, toId, usePc,       /* @06c13mtog-17805 */
                                           selPaln, ptr );  /* @06c13mtog-17805 */
                wafRetOnInit = 0;                           /* @06c13mtog-17805 */
                if( rtn )                                   /* @06c13mtog-17805 */
                {                                           /* @06c13mtog-17805 */
                    return( rtn );                          /* @06c13mtog-17805 */
                }                                           /* @06c13mtog-17805 */
			}
		}
/* @18a24rsas-41241        if( SysState == SYS_WAFSORT )                     @* @01411kish-5506 */
/* @18a24rsas-41241        {                                                 @* @01411kish-5506 */
/* Return WTBL wafer to CST */                            /* @01411kish-5506 */
/* @18a24rsas-41241            ptrWTbl = LdGetWafPtr( W_TBL, 0 );            @* @01411kish-5506 */
/* @18a24rsas-41241            if( ptrWTbl != WAF_EMPTY )                    @* @01411kish-5506 */
/* @18a24rsas-41241            {                                             @* @01411kish-5506 */
/* @18a24rsas-41241                if( LdWTblRdWaf( S_OFF ) != OK )          @* @01411kish-5506 */
/* @18a24rsas-41241                {                                         @* @01411kish-5506 */
/* @18a24rsas-41241                    toId = LdGetWafReturnCst( ptrWTbl );  @* @01411kish-5506 */
/* @18a24rsas-41241                    if(( toId != ERROR ) && ( toId & CST ))@* @01411kish-5506 */
/* @18a24rsas-41241                    {                                     @* @01411kish-5506 */
/* @18a24rsas-41241                        rtn = LdWafReturn( W_TBL, toId, UPPER, selPaln, ptrWTbl); @* @01411kish-5506 */
/* @18a24rsas-41241                        if( rtn )                         @* @01411kish-5506 */
/* @18a24rsas-41241                        {                                 @* @01411kish-5506 */
/* @18a24rsas-41241                            return( rtn );                @* @01411kish-5506 */
/* @18a24rsas-41241                        }                                 @* @01411kish-5506 */
/* @18a24rsas-41241                        wafRetOnInit = 0;                 @* @01411kish-5506 */
/* @18a24rsas-41241                    }                                     @* @01411kish-5506 */
/* @18a24rsas-41241                }                                         @* @01411kish-5506 */
/* @18a24rsas-41241            }                                             @* @01411kish-5506 */
/* @18a24rsas-41241        }                                                 @* @01411kish-5506 */

/* ウェーハテーブルウェーハ返却 */                                  /* @06c13mtog-17805 */
        if( LdWTblRdWaf(S_ON) == OK )                               /* @06c13mtog-17805 */
        {                                                           /* @06c13mtog-17805 */
            LdChkWTblWaf(WAF_EXIST,&ptrWTbl);                       /* @06c13mtog-17805 */
            ptr = SramGet4( SramWTblWaf ) ;                         /* @06c13mtog-17805 */
            if( (ptr != 0x00) && (ptr != 0xff) &&                   /* @06c13mtog-17805 */
                (ptr != (WTBL_WAF+1)) && (ptr != (BTBL_WAF+1)) )    /* @06c13mtog-17805 */
            {                                                       /* @06c13mtog-17805 */
                if( ptr < SYS_SLOT_MAX )                            /* @06c13mtog-17805 */
                {                                                   /* @06c13mtog-17805 */
                    cNo = 0;                                        /* @06c13mtog-17805 */
                }                                                   /* @06c13mtog-17805 */
                if( ptr >= SYS_SLOT_MAX )                           /* @06c13mtog-17805 */
                {                                                   /* @06c13mtog-17805 */
                    cNo = 1;                                        /* @06c13mtog-17805 */
                }                                                   /* @06c13mtog-17805 */
                if( OptionInfoGet(PO_2ND1CST) )                     /* @06c13mtog-17805 */
                {                                                   /* @06c13mtog-17805 */
                    cNo = 1;                                        /* @06c13mtog-17805 */
                }                                                   /* @06c13mtog-17805 */
            }                                                       /* @06c13mtog-17805 */

            if( (ptrWTbl >= 0) && (ptrWTbl != WTBL_WAF) )           /* @06c13mtog-17805 */
            {                                                       /* @06c13mtog-17805 */
                toId = LdGetWafReturnCst( ptrWTbl );                /* @06c13mtog-17805 */
                if( toId == ERROR )                                 /* @06c13mtog-17805 */
                {                                                   /* @06c13mtog-17805 */
                    toId = LdGetWafReturn( ptrWTbl );               /* @06c13mtog-17805 */
                    if( toId == ERROR )                             /* @06c13mtog-17805 */
                    {                                               /* @06c13mtog-17805 */
                        if( (ptr == 0x00) || (ptr == 0xff) ||       /* @06c13mtog-17805 */
                            (ptr == (WTBL_WAF+1)) ||                /* @06c13mtog-17805 */
                            (ptr == (BTBL_WAF+1)) )                 /* @06c13mtog-17805 */
                        {                                           /* @06c13mtog-17805 */
                            if( ptr == (BTBL_WAF+1) )               /* @06c13mtog-17805 */
                            {                                       /* @06c13mtog-17805 */
                                toId = B_TBL;                       /* @06c13mtog-17805 */
                            }                                       /* @06c13mtog-17805 */
                            else                                    /* @06c13mtog-17805 */
                            {                                       /* @06c13mtog-17805 */
                                toId = 0;                           /* @06c13mtog-17805 */
                            }                                       /* @06c13mtog-17805 */
                        }                                           /* @06c13mtog-17805 */
                        else                                        /* @06c13mtog-17805 */
                        {                                           /* @06c13mtog-17805 */
                            ptr --;                                 /* @06c13mtog-17805 */
                            toId = LdMapWafReturn( ptr, cNo );      /* @06c13mtog-17805 */
                            if( toId != 0 )                         /* @06c13mtog-17805 */
                            {                                       /* @06c13mtog-17805 */
                                ptrWTbl = ptr;                      /* @06c13mtog-17805 */
                            }                                       /* @06c13mtog-17805 */
                        }                                           /* @06c13mtog-17805 */
                    }                                               /* @06c13mtog-17805 */
                }                                                   /* @06c13mtog-17805 */

                if( toId == W_TBL )                                 /* @06c13mtog-17805 */
                {                                                   /* @06c13mtog-17805 */
                    goto WTBL_WAF_RET_END;                          /* @06c13mtog-17805 */
                }                                                   /* @06c13mtog-17805 */

                if( toId != 0 )                                     /* @06c13mtog-17805 */
                {                                                   /* @06c13mtog-17805 */
                    if( LdGetId(toId) == CST )                      /* @06c13mtog-17805 */
                    {                                               /* @06c13mtog-17805 */
                        cstNo = LdGetCstNo ( toId );                         /* @18a24rsas-41241 */
                        sltNo = LdGetSlotNo( toId );                         /* @18a24rsas-41241 */
                        if( MapDt[cstNo].mapEnd == 0 &&                      /* @18a24rsas-41241 */
                            LdUnldPosChk( cstNo, UPPER, sltNo ) == ERROR )   /* @18a24rsas-41241 */
                        {                                                    /* @18a24rsas-41241 */
                            toId = ChkRtnCalCstWaf(cstNo, sltNo );           /* @22b17hlam-T24080 */
                            if( toId == 0 )                                  /* @22b17hlam-T24080 */
                            {                                                /* @22b17hlam-T24080 */
                                LdErrMsg( WTBL_WAFER_EXIST, ERR_LOADER_ACK, 0 );  /* @18a24rsas-41241 */
                                goto WTBL_WAF_RET_END;                            /* @18a24rsas-41241 */
                            }                                                /* @22b17hlam-T24080 */
                        }                                                    /* @18a24rsas-41241 */
                        if( userNoGet() != INTEL )                  /* @06c13mtog-17805 */
                        {                                           /* @06c13mtog-17805 */
                            LdErrMsg( WTBL_RETURN_CST,              /* @06c13mtog-17805 */
                                          ERR_LOADER_ACK, 0 );      /* @06c13mtog-17805 */
                        }                                           /* @06c13mtog-17805 */
                        if( LdWTblRdWaf(S_ON) != OK )               /* @06c13mtog-17805 */
                        {                                           /* @06c13mtog-17805 */
                            goto WTBL_WAF_RET_END;                  /* @06c13mtog-17805 */
                        }                                           /* @06c13mtog-17805 */
                    }                                               /* @06c13mtog-17805 */

                    rtn = LdWafReturn( W_TBL, toId, UPPER,          /* @06c13mtog-17805 */
                                               selPaln, ptrWTbl );  /* @06c13mtog-17805 */
                    wafRetOnInit = 0;                               /* @06c13mtog-17805 */
                    if( rtn )                                       /* @06c13mtog-17805 */
                    {                                               /* @06c13mtog-17805 */
                        return( rtn );                              /* @06c13mtog-17805 */
                    }                                               /* @06c13mtog-17805 */
                }                                                   /* @06c13mtog-17805 */
            }                                                       /* @06c13mtog-17805 */
            else                                                    /* @06c13mtog-17805 */
            {                                                       /* @06c13mtog-17805 */
				if( userNoGet() != INTEL )                          /* @06c13mtog-17805 */
				{                                                   /* @06c13mtog-17805 */
					LdErrMsg( WTBL_WAFER_EXIST, ERR_LOADER_ACK, 0 );/* @06c13mtog-17805 */
				}                                                   /* @06c13mtog-17805 */
            }                                                       /* @06c13mtog-17805 */
        }                                                           /* @06c13mtog-17805 */
WTBL_WAF_RET_END:                                                   /* @06c13mtog-17805 */
        LdSetWTblWaf( WAF_EMPTY );                                  /* @06c13mtog-17805 */
        SramSet4( SramWTblWaf, 0 );                                 /* @06c13mtog-17805 */
#endif                                                    /* @9c02mtog-625 */
	}
	else
	{
		sortErr( ) ;
/* @09526rame-23042        if( ( rtn = SimpleInit() ) )                         @* @9223kish-01 */
        rtn = SimpleInit( scExistErr, scwaf );                  /* @09526rame-23042 */
        if( rtn != OK )                                         /* @09526rame-23042 */
		{
			return( rtn ) ;
		}
	}

    if( OptionInfoGet( PO_BERNOULLI_ARM ) == 1 )                        /* @09526rame-23042 */
    {                                                                   /* @09526rame-23042 */
        rtn = LdChkWafPalnLED( 4 );                                     /* @09526rame-23042 */

        if( rtn == WAF_EXIST )                                          /* @09526rame-23042 */
        {                                                               /* @09526rame-23042 */
            LdRemoveWaf_Hand();                                         /* @09526rame-23042 */
        }                                                               /* @09526rame-23042 */

        SramSet1( Pc2ScWafMove, NOMOVE );  /* SRAM clear */             /* @09526rame-23042 */
    }                                                                   /* @09526rame-23042 */

    if(!OptionInfoGet( PO_SUBCHUCKZ ))                       /* @00602kish-4053 */
    {                                                        /* @00602kish-4053 */
    if( ( rtn = LdScUp() ) )                                 /* @9223kish-01 */
    {                                                        /* @9223kish-01 */
        return( rtn );                                       /* @9223kish-01 */
    }                                                        /* @9223kish-01 */
    if( ( rtn = LdScDn() ) )                                 /* @9223kish-01 */
    {                                                        /* @9223kish-01 */
        return( rtn );                                       /* @9223kish-01 */
    }                                                        /* @9223kish-01 */
    }                                                        /* @00602kish-4053 */

#if LD_SPEC_SMIF                                          /* @9c02mtog-625 */
    if( (MtrRdStatus(MTR_UPC, 2) & MTR2_ORG_BIT) == 0 )   /* @9c02mtog-625 */
    {                                                     /* @9c02mtog-625 */
        return( (LdSetErr(IDX_UPC_NOT_HOME, 0)) );        /* @9c02mtog-625 */
    }                                                     /* @9c02mtog-625 */
    if( (MtrRdStatus(MTR_LPC, 2) & MTR2_ORG_BIT) == 0 )   /* @9c02mtog-625 */
    {                                                     /* @9c02mtog-625 */
        return( (LdSetErr(IDX_LPC_NOT_HOME, 0)) );        /* @9c02mtog-625 */
    }                                                     /* @9c02mtog-625 */
    LdPartDt.idxPos = 0;                                  /* @9c02mtog-625 */
    memset( &rcvBufSmf[0], 0x00, sizeof(rcvBufSmf) );     /* @9c02mtog-625 */
    memset( &sndBufSmf[0], 0x00, sizeof(sndBufSmf) );     /* @9c02mtog-625 */
    strcpy( &sndBufSmf[0], "LIXMVHM" );                   /* @9c02mtog-625 */
    MQSend( IdxControlTaskID, &sndBufSmf[0], WAIT_FOREVER,/* @9c02mtog-625 */
                                                      0 );/* @9c02mtog-625 */
    MQReceive( &fTaskSmf, &tTaskSmf, &paraNumSmf,         /* @9c02mtog-625 */
               &rcvBufSmf[0], WAIT_FOREVER, &ackComSmf,   /* @9c02mtog-625 */
                                         &ackBufSmf[0] ); /* @9c02mtog-625 */
    MQGetNo( &rcvBufSmf[0], &rtn, 2 );                    /* @9c02mtog-625 */
/* ポッドが無いときはホーム駆動やステージ駆動は”ポッド      @9c02mtog-625 */
/* 無し(HCACK=65)”でエラー扱いみたいです。ちなみにポッドが  @9c02mtog-625 */
/* 無いときは、LIXMVHMの返値は”１5”となるようです。        @9c02mtog-625 */
/*（と言うよりRCMDでｲﾝﾃﾞｸｻｰ駆動に関するものについては、全て  @9c02mtog-625 */
/* HCACK=65が返りそうなので、要注意です。）                  @9c02mtog-625 */
/* イニシャル時に、あらかじめポッドが無い時はエラーにする    @9c02mtog-625 */
/* べきではない                                              @9c02mtog-625 */
    if( ( rtn != 0 ) && ( rtn != 15 ) )                   /* @9c02mtog-625 */
    {                                                     /* @9c02mtog-625 */
        LdErrRstSmf( );                                   /* @9c02mtog-625 */
        if( (rtn == 10) || (rtn == 20) )                  /* @9c02mtog-625 */
        {                                                 /* @9c02mtog-625 */
            setErr = SMIF_CONNECT_ERR;                    /* @9c02mtog-625 */
        }                                                 /* @9c02mtog-625 */
        else                                              /* @9c02mtog-625 */
        {                                                 /* @9c02mtog-625 */
            setErr = SMIF_IDX_DRV_ERR;                    /* @9c02mtog-625 */
        }                                                 /* @9c02mtog-625 */
        return( LdSetErr(setErr, 0) );                    /* @9c02mtog-625 */
    }                                                     /* @9c02mtog-625 */
    LdPartDt.idxPos = IDX_HM;                             /* @9c02mtog-625 */
    rtn = LdGetIdxAddr( &IDXHome, &IDXHomeMil );          /* @00b14mtog-5061 */
    if( rtn )                                             /* @00b14mtog-5061 */
    {                                                     /* @00b14mtog-5061 */
        errRtn = LdErrMsg( SMIF_CONNECT_ERR,              /* @00b14mtog-5061 */
                                   ERR_SYSTEM_RESET, 0 ); /* @00b14mtog-5061 */
    }                                                     /* @00b14mtog-5061 */
    LdSetErr( 0,0 );                                      /* @9c02mtog-625 */
#else                                                     /* @9c02mtog-625 */

E_RETRY_Clp:
/* インデクサ停止仕様 	Written by M.Togawa  '96.05.23 */
	if( (LdCstRdSet(0, 1) == OK) || (LdCstRdSet(1, 1) == OK) )
		MQSend( LdIdxStopTask, "CHK_CLP", WAIT_FOREVER, 0 ) ;
	rtn = LdIdxGoHm ();
/* インデクサ停止仕様 	Written by M.Togawa  '96.05.23 */
	MQSend( LdIdxStopTask, "END_CLP", WAIT_FOREVER, 0 ) ;
	if ( rtn )
	{
		if( rtn == CST_NOT_CLAMP1 + 2000 )
		{
			LdCstRls( 0, 0 ) ;
		}
		else if( rtn == CST_NOT_CLAMP2 + 2000 )
		{
			LdCstRls( 1, 0 ) ;
		}
		else	return( rtn ) ;

        errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 );
        if ( errRtn == ERR_RETRY_MSG )
		{
			if( rtn == CST_NOT_CLAMP1 + 2000 )
			{
    			intV = CstRls [ 0 ].intV;
    			cTime= CstRls [ 0 ].cTime;

                DrvSens( LdCstWtRls, 2, 0, S_OFF,
                      LdCstRdRls,2, 0, S_OFF, 0 , intV, cTime , 0 );
			}
			else if( rtn == CST_NOT_CLAMP2 + 2000 )
			{
    			intV = CstRls [ 1 ].intV;
    			cTime= CstRls [ 1 ].cTime;

                DrvSens( LdCstWtRls, 2, 1, S_OFF,
                      LdCstRdRls,2, 1, S_OFF, 0 , intV, cTime , 0 );
			}
			goto E_RETRY_Clp;
		}
        if ( errRtn == LD_DIAG_ERR ) return ( rtn );
	}

	if( OptionInfoGet( PO_2ND1CST ) )
	{
        if( ( rtn = LdCstFlat( 1, 0 ) ) )                    /* @9223kish-01 */
        {                                                    /* @9223kish-01 */
            return( rtn );                                   /* @9223kish-01 */
        }                                                    /* @9223kish-01 */
	}
	else
	{
        if( ( rtn = LdCstFlat( 0, 0 ) ) )                    /* @9223kish-01 */
        {                                                    /* @9223kish-01 */
            return( rtn );                                   /* @9223kish-01 */
        }                                                    /* @9223kish-01 */
	}

	if( OptionInfoGet( PO_2NDLOADER ) )
	{
        if( ( rtn = LdCstFlat( 1, 0 ) ) )                    /* @9223kish-01 */
        {                                                    /* @9223kish-01 */
            return( rtn );                                   /* @9223kish-01 */
        }                                                    /* @9223kish-01 */
	}
#endif                                                    /* @9c02mtog-625 */

	/* ローダイニシャル終了で、ピンセットＺをアップ 1995.2.2 */
    if( ( rtn = LdPzMove( 0 ) ) )                            /* @9223kish-01 */
    {                                                        /* @9223kish-01 */
        return( rtn );                                       /* @9223kish-01 */
    }                                                        /* @9223kish-01 */

#if( !LD_SPEC_SMIF )                             /* @08701mtog-22229 */
    if( ChkLdBernoulliWithSwitchUnit() == TRUE ) /* @08701mtog-22229 */
    {                                            /* @08701mtog-22229 */
        rtn = LdInitPcChgUnit( wafSize );        /* @08701mtog-22229 */
        if( rtn != OK )                          /* @08701mtog-22229 */
        {                                        /* @08701mtog-22229 */
            return( rtn );                       /* @08701mtog-22229 */
        }                                        /* @08701mtog-22229 */
    }                                            /* @08701mtog-22229 */

/* @09722kish-24778    if( OptionInfoGet( PO_BERNOULLI_ARM ) == 1 ) @* @09722kish-24778 */
    if( LdChkBerAirPress_Pc() == 1 )             /* @18b13rame-41195 */
    {                                            /* @09722kish-24778 */
        if(( rtn = LdExhaustPcAir( UPPER ) ))    /* @09722kish-24778 */
        {                                        /* @09722kish-24778 */
            return( rtn );                       /* @09722kish-24778 */
        }                                        /* @09722kish-24778 */
                                                 /* @09722kish-24778 */
        if(( rtn = LdExhaustPcAir( LOWER ) ))    /* @09722kish-24778 */
        {                                        /* @09722kish-24778 */
            return( rtn );                       /* @09722kish-24778 */
        }                                        /* @09722kish-24778 */
    }                                            /* @09722kish-24778 */

    if( airContSc == 1 )                         /* @18b13rame-41195 */
    {                                            /* @18b13rame-41195 */
        if(( rtn = LdExhaustScAir() ))           /* @18b13rame-41195 */
        {                                        /* @18b13rame-41195 */
            return( rtn );                       /* @18b13rame-41195 */
        }                                        /* @18b13rame-41195 */
    }                                            /* @18b13rame-41195 */
#endif                                           /* @08701mtog-22229 */

	return ( OK );
}

/*#==================================================================#*/
/*#        Function    : SimpleInit									 #*/
/*#        Summary     : #*/
/*#        Argument    : #*/
/*#        Return      : #*/
/*#        Extern      : #*/
/*#        Caution     : #*/
/*#			サブチャックのドライブはしない						     #*/
/*#			カセットフラットはしない								 #*/
/*#==================================================================#*/
/* @09526rame-23042 STATUS	SimpleInit ()  */
STATUS  SimpleInit( intl scExistErr, intl wafInfo )            /* @09526rame-23042 */
{
	intl	rtn;
/* @01b20kish-7382	intl	cNo;     */
	intl	dWait;
	intl	intV;
	intl	cTime;
	intl	errRtn;
/* @01b20kish-7382	intl	rtnERR;  */
	intl	sta ;
#if LD_SPEC_SMIF                                           /* @9c02mtog-625 */
    intl    setErr;                                        /* @9c02mtog-625 */
#else                                                      /* @9c02mtog-625 */
/* @18b13rame-41195	intl	cNo;                                    @* @01b20kish-7382 */
	intl	rtnERR;                                 /* @01b20kish-7382 */
#endif                                                          /* @09526rame-23042 */
/* @12b16rame-29941 #if( P_OCTO )                                       @* @12723rame-29272 */
/* @12b16rame-29941     intl    nzlHome;                                @* @12723rame-29272 */
/* @12b16rame-29941     intl    nzlSet;                                 @* @12723rame-29272 */
/* @12b16rame-29941 #endif                                              @* @12723rame-29272 */

    intl    bernolliArm;                                        /* @09526rame-23042 */
    intl    subchuckZMtr = 0;                                   /* @12808ykor-29900 */ 
    intl    pcWafOnFlg = 0;                                     /* @12808ykor-29900 : P-F1 */ 
    intl    scSuctionType = 0;                                  /* @14730rame-33536 */

    intl    cNo = 0;                                            /* @18b13rame-41195 */
    intl    airContBkPc = 0;                                    /* @18b13rame-41195 */
    intl    airContSc = 0;                                      /* @18b13rame-41195 */

#if( P_OCTO )                                           /* @21524thos-42311 */
    LD_STG_SAFE = 0;              /* @13425tnak-30599 *//* @21524thos-42311 */
#endif                                                  /* @21524thos-42311 */

    bernolliArm = OptionInfoGet(PO_BERNOULLI_ARM);              /* @09526rame-23042 */
    subchuckZMtr = OptionInfoGet( PO_SUBCHUCKZ );               /* @12808ykor-29900 */
    scSuctionType = ChkSuctionType_Sc();                        /* @14730rame-33536 */

    /* Air圧力制御機能の設定状態を取得           @18b13rame-41195 */
    airContBkPc = chkBkSideBer_PcAirCont();   /* @18b13rame-41195 */
    airContSc = chkBer_ScAirCont();           /* @18b13rame-41195 */

#if ( !LD_SPEC_SMIF )                                           /* @09526rame-23042 */
/* Slide initial sequence is moved in front of Pc initial sequence. *//* @9602kish-1768 */
    /**********************************************/         /* @9602kish-1768 */
    /* ファースト仕様は、原点センサは必ずオン     */         /* @9602kish-1768 */
    /* 原点センサオフ処理は、既に終了しているはず */         /* @9602kish-1768 */
    /**********************************************/         /* @9602kish-1768 */
    /* チルトのままでは、イニシャルしてはいけない */         /* @9602kish-1768 */
    if( ( rtn = LdCstFlat( 1, 0 ) ) )                        /* @9602kish-1768 */
    {                                                        /* @9602kish-1768 */
        return( rtn );                                       /* @9602kish-1768 */
    }                                                        /* @9602kish-1768 */
                                                             /* @9602kish-1768 */
    if( LdSldRdOrg( S_ON ) == OK )                           /* @9602kish-1768 */
    {                                                        /* @9602kish-1768 */
            if( OptionInfoGet( PO_2NDLOADER )                /* @9602kish-1768 */
                   || OptionInfoGet( PO_2ND1CST ) )          /* @9602kish-1768 */
            {                                                /* @9602kish-1768 */
E_RETRY4:                                                    /* @9602kish-1768 */
                if( ( rtnERR = LdSldGoOrg() ) )              /* @9602kish-1768 */
                {                                            /* @9602kish-1768 */
/* カセットセットセンサがオンであれば、クランプを実行します*//* @9602kish-1768 */
E_RETRY_SP0:                                                 /* @9602kish-1768 */
                    if( LdCstRdSet( 1, S_ON ) == OK )        /* @9602kish-1768 */
                    {                                        /* @9602kish-1768 */
                        if( ( rtn = LdCstClp( 1, 0 ) ) )     /* @9602kish-1768 */
                        {                                    /* @9602kish-1768 */
                            goto E_RETRY_SP0;                /* @9602kish-1768 */
                        }                                    /* @9602kish-1768 */
                    }                                        /* @9602kish-1768 */
                    else                                     /* @9602kish-1768 */
                    {                                        /* @9602kish-1768 */
                        if( ( rtn = LdCstRls( 1, 0 ) ) )     /* @9602kish-1768 */
                        {                                    /* @9602kish-1768 */
                            goto E_RETRY_SP0;                /* @9602kish-1768 */
                        }                                    /* @9602kish-1768 */
                    }                                        /* @9602kish-1768 */
                                                             /* @9602kish-1768 */
                    errRtn = LdErrMsg( rtnERR, ERR_LOADER_RETRY,0 ); /* @9602kish-1768 */
                    if( errRtn == ERR_RETRY_MSG )            /* @9602kish-1768 */
                    {                                        /* @9602kish-1768 */
                        goto E_RETRY4;                       /* @9602kish-1768 */
                    }                                        /* @9602kish-1768 */
                    if( errRtn == LD_DIAG_ERR )              /* @9602kish-1768 */
                    {                                        /* @9602kish-1768 */
                        return( rtn );                       /* @9602kish-1768 */
                    }                                        /* @9602kish-1768 */
                }                                            /* @9602kish-1768 */
                if( ( rtn = LdSldGoHm() ) )                  /* @9602kish-1768 */
                {                                            /* @9602kish-1768 */
                    errRtn = LdErrMsg( rtn, ERR_LOADER_RETRY,0 ); /* @9602kish-1768 */
                    if( errRtn == ERR_RETRY_MSG )            /* @9602kish-1768 */
                    {                                        /* @9602kish-1768 */
                        goto E_RETRY4;                       /* @9602kish-1768 */
                    }                                        /* @9602kish-1768 */
                    if( errRtn == LD_DIAG_ERR )              /* @9602kish-1768 */
                    {                                        /* @9602kish-1768 */
                        return( rtn );                       /* @9602kish-1768 */
                    }                                        /* @9602kish-1768 */
                }                                            /* @9602kish-1768 */
            }                                                /* @9602kish-1768 */
            else                                             /* @9602kish-1768 */
            {                                                /* @9602kish-1768 */
                /* クランプは、ものがなくてもＯＫ */         /* @9602kish-1768 */
                LdSldWtClp( S_ON );                          /* @9602kish-1768 */
            }                                                /* @9602kish-1768 */
    }                                                        /* @9602kish-1768 */
    else                                                     /* @9602kish-1768 */
    {                                                        /* @9602kish-1768 */
        /**********************************************************************//* @9602kish-1768 */
        /* スライド原点センサがオフの場合は、オペレータにイニシャルしてもらう *//* @9602kish-1768 */
        /* ファースト仕様は、原点センサは必ずオン したがって、オフになるという*//* @9602kish-1768 */
        /* ことはセカンド仕様                                                 *//* @9602kish-1768 */
        /**********************************************************************//* @9602kish-1768 */
        /* クランプを解除する */                             /* @9602kish-1768 */
        if( ( rtn = LdCstRls( 1, 0 ) ) )                     /* @9602kish-1768 */
        {                                                    /* @9602kish-1768 */
            return( rtn );                                   /* @9602kish-1768 */
        }                                                    /* @9602kish-1768 */
                                                             /* @9602kish-1768 */
/* @08910ykor-23437        if( ( MtrRdStatus( MTR_IDX, 2 ) & MTR2_NORG_BIT )    @* @9602kish-1768 */
/* @08910ykor-23437                                          != MTR2_NORG_BIT ) @* @9602kish-1768 */
        if( LdIdxRdOrg() == S_OFF )                          /* @08910ykor-23437 */
        {                                                    /* @9602kish-1768 */
            return( LdSetErr( SLD_IDX_NOT_HOME, 0 ) );       /* @9602kish-1768 */
        }                                                    /* @9602kish-1768 */
                                                             /* @9602kish-1768 */
         LdErrMsg ( SLD_ORG_MOVE, ERR_LOADER_ACK,0 );        /* @9602kish-1768 */
/* カセットセットセンサがオンであれば、クランプを実行します*//* @9602kish-1768 */
E_RETRY_SP:                                                  /* @9602kish-1768 */
        if( LdCstRdSet( 1, S_ON ) == OK )                    /* @9602kish-1768 */
        {                                                    /* @9602kish-1768 */
            if( ( rtn = LdCstClp( 1, 0 ) ) )                 /* @9602kish-1768 */
            {                                                /* @9602kish-1768 */
                goto E_RETRY_SP;                             /* @9602kish-1768 */
            }                                                /* @9602kish-1768 */
        }                                                    /* @9602kish-1768 */
        else                                                 /* @9602kish-1768 */
        {                                                    /* @9602kish-1768 */
            if( ( rtn = LdCstRls( 1, 0 ) ) )                 /* @9602kish-1768 */
            {                                                /* @9602kish-1768 */
                goto E_RETRY_SP;                             /* @9602kish-1768 */
            }                                                /* @9602kish-1768 */
        }                                                    /* @9602kish-1768 */
E_RETRY5:                                                    /* @9602kish-1768 */
        if( ( rtn = LdSldGoOrg() ) )                         /* @9602kish-1768 */
        {                                                    /* @9602kish-1768 */
            errRtn = LdErrMsg( rtn, ERR_LOADER_RETRY,0 );    /* @9602kish-1768 */
            if( errRtn == ERR_RETRY_MSG )                    /* @9602kish-1768 */
            {                                                /* @9602kish-1768 */
                goto E_RETRY5;                               /* @9602kish-1768 */
            }                                                /* @9602kish-1768 */
            if( errRtn == LD_DIAG_ERR )                      /* @9602kish-1768 */
            {                                                /* @9602kish-1768 */
                return( rtn );                               /* @9602kish-1768 */
            }                                                /* @9602kish-1768 */
        }                                                    /* @9602kish-1768 */
        if( ( rtn = LdSldGoHm() ) )                          /* @9602kish-1768 */
        {                                                    /* @9602kish-1768 */
            errRtn = LdErrMsg( rtn, ERR_LOADER_RETRY,0 );    /* @9602kish-1768 */
            if( errRtn == ERR_RETRY_MSG )                    /* @9602kish-1768 */
            {                                                /* @9602kish-1768 */
                goto E_RETRY5;                               /* @9602kish-1768 */
            }                                                /* @9602kish-1768 */
            if( errRtn == LD_DIAG_ERR )                      /* @9602kish-1768 */
            {                                                /* @9602kish-1768 */
                return( rtn );                               /* @9602kish-1768 */
            }                                                /* @9602kish-1768 */
        }                                                    /* @9602kish-1768 */
    }                                                        /* @9602kish-1768 */
#endif                                                     /* @9c02mtog-625 */

/* '95.09.18 modified by c.ogino
  オプションの設定に関わらず、シャッターが有るか否かを確認します。
  シャッター有りの判断は、以下の通り
		$10C-D3 bit		ShtExist
			0				0 	（シャッター無し）
			1				1 	（シャッター有り）
  これまでのロジックでもよいが、Ｐー８がリブートしたとき、
  なぜか？ブリッジがイニシャルでも退避しないため、シャッターのないマシンでも
  ”BRG_NOT_HOME”が出てしまい、問題になりました。
  このため、＄１０ＣーＤ３ビットを尊重したロジックに変更します。
  シャッターがある場合は、シャッターを開きます。
  （ スタンバイにいる状態の時に、LdPcGoOrgが掛かると、オリジンセンサー用の
     シャッター分だけ、ステージ側にピンセットを出してしまう為。）
  以後、シャッターの有り無しは ShtExist を参照します。
  ShtExist は、初期値 -99999（パワーオン） とし、
  判断した後は、上記表の値を持ちます。
*/
	if ( ShtExist < 0 )			/* == -99999 : パワーオン時のイニシャル */
	{
		sta = LdRd10c () ;	

		if ( ( sta & S_SHT_EXIST ) == S_SHT_EXIST ) /* ｼｬｯﾀｰ有無ビットが１ */
		{
			ShtExist = 1 ; /* シャッターあり */

			/* ブリッジが移動するまで待ちます。 */
/* @06306tkaw-16532	taskDelay ( TmGetAuxTickS ( 5 ) ) ;    */

			if ( StBrgRdSol () == ERROR ) /* ブリッジが退避位置にない時 */
			{
/* @8924kish-01				return ( LdSetErr ( BRG_NOT_HOME ) ) ; @* エラー処理してリターンする *@ */
				return( LdSetErr( BRG_NOT_HOME, 0 ) );/* エラー処理してリターンする *//* @8924kish-01 */
   			}

			LdShtWtOpn () ;		/* シャッターを開きます */
			taskDelay ( TmGetAuxTickS ( 5 ) ) ;

#if( P_OCTO )                                               /* @08930kisi-22903 */
            sta = NioReadL( LR_LIGHT_SHT ) ;                /* @08930kisi-22903 */
#else                                                       /* @08930kisi-22903 */
            sta = LdRd10c () ;
#endif                                                      /* @08930kisi-22903 */

			if ( ( sta & S_SHT_CLOSE ) == S_SHT_CLOSE )
			{
				return ( LdSetErr ( SHT_NOT_OPEN,0 ) ) ; /* ｼｬｯﾀｰが開かない・・・ */
			}
		}
		else									 /* ｼｬｯﾀｰ有無ビットが０ */
		{
			/* S_SHT_EXISTが０なので、シャッターなしと判断します。
			 もし、ピンセットがシャッターに衝突したら、ジャンパーコードの
			 未装着が考えられます。
			*/
			ShtExist = 0 ; /* シャッターなし */
			logMsg ( "\n Not SHUTTER-JUMPER-LINE\n" ) ;
		}

	}	/* == -99999 END */
	else
	{
		if ( ShtExist == 1 )	/* 通常のイニシャル */
		{
			/* ブリッジが移動するまで待ちます。 */
/* @06306tkaw-16532	taskDelay ( TmGetAuxTickS ( 5 ) ) ;  */

			if ( StBrgRdSol () == ERROR ) /* ブリッジが退避位置にない時 */
			{
				return( LdSetErr( SHT_NOT_OPEN, 0 ) ); /* エラー処理してリターンする *//* @8924kish-01 */
   			}

			rtn = LdShtRdSol ( 1 ) ;	/* シャッター開いている・・・？ */
			if ( rtn != OK )			/* 開いてない！！ */
			{
                if( ( rtn = LdShtOpnSens( 1 ) ) )            /* @9223kish-01 */
                {                                            /* @9223kish-01 */
                    return( rtn ) ;                          /* @9223kish-01 */
                }                                            /* @9223kish-01 */
			}

			/* 念のため、ディレイを入れておく */
			taskDelay ( TmGetAuxTickS ( 1 ) );

		}	/* ShtExist == 1 （通常のイニシャル） END */
	}	/* != -99999 END */

#if( P_OCTO )                                        /* @11530rame-27486 */
    if( LaserPalnUnit == LASER_PALN )                /* @11530rame-27486 */
    {                                                /* @11530rame-27486 */
        rtn = OLdInitLaserPalnUnit();                /* @11530rame-27486 */
        if( rtn )                                    /* @11530rame-27486 */
        {                                            /* @11530rame-27486 */
            return( LdSetErr( rtn, 0 ) );            /* @11530rame-27486 */
        }                                            /* @11530rame-27486 */
    }                                                /* @11530rame-27486 */

    if( WarpedLoader == TRUE )                            /* @12723rame-29272 */
    {                                                     /* @12723rame-29272 */
        /* Nozzle HOME Pos drive. */                      /* @12723rame-29272 */
/* @12b16rame-29941        rtn = LdIdxNozzlePos( NZL_HOME, 1 );              @* @12723rame-29272 */
        rtn = LdIdxNozzleHome_Initial( CST_0 );               /* @12b16rame-29941 */

        if(( OptionInfoGet( PO_2NDLOADER ) )                  /* @12b16rame-29941 */
        || ( OptionInfoGet( PO_2ND1CST ) ))                   /* @12b16rame-29941 */
        {                                                     /* @12b16rame-29941 */
            if( rtn == OK )                                   /* @12b16rame-29941 */
            {                                                 /* @12b16rame-29941 */
                rtn = LdIdxNozzleHome_Initial( CST_1 );       /* @12b16rame-29941 */
            }                                                 /* @12b16rame-29941 */
        }                                                     /* @12b16rame-29941 */

        if( rtn != OK )                                   /* @12723rame-29272 */
        {                                                 /* @12723rame-29272 */
            return( rtn );                                /* @12723rame-29272 */
        }                                                 /* @12723rame-29272 */

/* @12b16rame-29941        @* Nozzle HOME & SET Pos check. *@                @* @12723rame-29272 */
/* @12b16rame-29941        nzlHome =  LdIdxNozzleRdSol( NZL_HOME );          @* @12723rame-29272 */
/* @12b16rame-29941        nzlSet = LdIdxNozzleRdSol( NZL_SET );             @* @12723rame-29272 */

/* @12b16rame-29941        if(!(( nzlHome == OK ) && ( nzlSet == ERROR )))   @* @12723rame-29272 */
/* @12b16rame-29941        {                                                 @* @12723rame-29272 */
/* @12b16rame-29941            @* Nozzleのhome位置sensorがON、 *@            @* @12723rame-29272 */
/* @12b16rame-29941            @* set位置sensorがOFF以外の状態 *@            @* @12723rame-29272 */
/* @12b16rame-29941            return( LdSetErr( IDX_NOZZLE_NOT_HOME, 0 ) ); @* @12723rame-29272 */
/* @12b16rame-29941        }                                                 @* @12723rame-29272 */
    }                                                     /* @12723rame-29272 */
#endif                                               /* @11530rame-27486 */

    if( OptionInfoGet(PO_BERNOULLI_WAFBACK) )         /* @06527hyod-17006 */
    {                                                 /* @06527hyod-17006 */
        if( scSuctionType == BERNOULLI_SC )           /* @14730rame-33536 */
        {                                             /* @14730rame-33536 */
            if( airContSc == 1 )                                              /* @18b13rame-41195 */
            {                                                                 /* @18b13rame-41195 */
                if(( rtn = LdScAirPresCont_OnOff( S_OFF, AIR_PRES_CONT,       /* @18b13rame-41195 */
                                  PcAirPresDt.PcAirCst[cNo].scPcAir,          /* @18b13rame-41195 */
                                  PcAirPresDt.PcAirCst[cNo].scPcAirTime )))   /* @18b13rame-41195 */
                {                                                             /* @18b13rame-41195 */
                    return( rtn );       /* Air圧力制御NG */                  /* @18b13rame-41195 */
                }                                                             /* @18b13rame-41195 */
            }                                                                 /* @18b02asug-41195 */
            else                                                              /* @18b02asug-41195 */
            {                                                                 /* @18b02asug-41195 */
                LdScWtVac ( S_OFF );                          /* @06527hyod-17006 */
            }                                                                 /* @18b13rame-41195 */
        }                                             /* @14730rame-33536 */
        else                                          /* @14730rame-33536 */
        {                                             /* @14730rame-33536 */
            if(( rtn = LdScVacOff( 1 ) ))             /* @14730rame-33536 */
            {                                         /* @14730rame-33536 */
                return( rtn );                        /* @14730rame-33536 */
            }                                         /* @14730rame-33536 */
        }                                             /* @14730rame-33536 */

        if( airContBkPc == 1 )                                                  /* @18b13rame-41195 */
        {                                                                       /* @18b13rame-41195 */
            if(( rtn = LdPcAirPresCont_OnOff( UPPER, S_ON, AIR_PRES_CONT,       /* @18b13rame-41195 */
                              PcAirPresDt.PcAirCst[cNo].pcScAir[UPPER],         /* @18b13rame-41195 */
                              PcAirPresDt.PcAirCst[cNo].pcScAirTime[UPPER] )))  /* @18b13rame-41195 */
            {                                                                   /* @18b13rame-41195 */
                return( rtn );       /* Air圧力制御NG */                        /* @18b13rame-41195 */
            }                                                                   /* @18b13rame-41195 */
            if(( rtn = LdPcAirPresCont_OnOff( LOWER, S_ON, AIR_PRES_CONT,       /* @18b13rame-41195 */
                              PcAirPresDt.PcAirCst[cNo].pcScAir[LOWER],         /* @18b13rame-41195 */
                              PcAirPresDt.PcAirCst[cNo].pcScAirTime[LOWER] )))  /* @18b13rame-41195 */
            {                                                                   /* @18b13rame-41195 */
                return( rtn );       /* Air圧力制御NG */                        /* @18b13rame-41195 */
            }                                                                   /* @18b13rame-41195 */
        }                                                                       /* @18b13rame-41195 */
        else                                                                    /* @18b13rame-41195 */
        {                                                                       /* @18b13rame-41195 */
            LdPcWtVac ( UPPER, S_ON );                    /* @06527hyod-17006 */
            LdPcWtVac ( LOWER, S_ON );                    /* @06527hyod-17006 */
        }                                                                       /* @18b13rame-41195 */
    }                                                 /* @06527hyod-17006 */

E_RETRY0:
    if( ( rtn = LdPcGoOrg( UPPER ) ) )                       /* @9223kish-01 */
	{
        errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 );
        if ( errRtn == ERR_RETRY_MSG ) goto E_RETRY0;
        if ( errRtn == LD_DIAG_ERR ) return ( rtn );
    }
    if( ( rtn = LdPcGoHm( UPPER ) ) )                        /* @9223kish-01 */
	{
        errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 );
        if ( errRtn == ERR_RETRY_MSG ) goto E_RETRY0;
        if ( errRtn == LD_DIAG_ERR ) return ( rtn );
	}
E_RETRY1:
    if( ( rtn = LdPcGoOrg( LOWER ) ) )                       /* @9223kish-01 */
	{
        errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 );
        if ( errRtn == ERR_RETRY_MSG ) goto E_RETRY1;
        if ( errRtn == LD_DIAG_ERR ) return ( rtn );
	}
    if( ( rtn = LdPcGoHm( LOWER ) ) )                        /* @9223kish-01 */
	{
        errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 );
        if ( errRtn == ERR_RETRY_MSG ) goto E_RETRY1;
        if ( errRtn == LD_DIAG_ERR ) return ( rtn );
	}

    if( bernolliArm == TRUE )                                   /* @09526rame-23042 */
    {                                                           /* @09526rame-23042 */
        rtn = LdReturnScWaf_Brnl( scExistErr, wafInfo );        /* @09526rame-23042 */
        if( rtn != OK )                                         /* @09526rame-23042 */
        {                                                       /* @09526rame-23042 */
            return( LdSetErr(rtn, 0) );                         /* @09526rame-23042 */
        }                                                       /* @09526rame-23042 */
    }                                                           /* @09526rame-23042 */

    if( OptionInfoGet( PO_SUBCHUCKZ ))                         /* @06527hyod-17006 */
    {                                                          /* @06527hyod-17006 */
        if( OptionInfoGet(PO_BERNOULLI_WAFBACK) == 0 )         /* @06527hyod-17006 */
        {                                                      /* @06527hyod-17006 */
            if( scSuctionType == BERNOULLI_SC )                /* @14730rame-33536 */
            {                                                  /* @14730rame-33536 */
                LdScWtVac( S_OFF );                            /* @14730rame-33536 */
            }                                                  /* @14730rame-33536 */
            else                                               /* @14730rame-33536 */
            {                                                  /* @14730rame-33536 */
                if( ( rtn = LdScVacOff( 1 ) ) )                    /* @06527hyod-17006 */
                {                                                  /* @06527hyod-17006 */
                    return( rtn );                                 /* @06527hyod-17006 */
                }                                                  /* @06527hyod-17006 */
            }                                                  /* @14730rame-33536 */
        }                                                      /* @06527hyod-17006 */
E_RETRY8:                                                      /* @06527hyod-17006 */
        if( ( rtn = LdScZGoOrg() ) )                           /* @06527hyod-17006 */
        {                                                      /* @06527hyod-17006 */
            errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 );     /* @06527hyod-17006 */
            if ( errRtn == ERR_RETRY_MSG ) goto E_RETRY8;      /* @06527hyod-17006 */
            if ( errRtn == LD_DIAG_ERR ) return ( rtn );       /* @06527hyod-17006 */
        }                                                      /* @06527hyod-17006 */
        if( ( rtn = LdScZGoHm() ) )                            /* @06527hyod-17006 */
        {                                                      /* @06527hyod-17006 */
            errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 );     /* @06527hyod-17006 */
            if ( errRtn == ERR_RETRY_MSG ) goto E_RETRY8;      /* @06527hyod-17006 */
            if ( errRtn == LD_DIAG_ERR ) return ( rtn );       /* @06527hyod-17006 */
        }                                                      /* @06527hyod-17006 */
    }                                                          /* @06527hyod-17006 */

    if( OptionInfoGet(PO_BERNOULLI_WAFBACK) )                  /* @06527hyod-17006 */
    {                                                          /* @06527hyod-17006 */
        if( OptionInfoGet( PO_SUBCHUCKZ ) == 0 )               /* @06527hyod-17006 */
        {                                                      /* @06527hyod-17006 */
            LdScDn ();                                         /* @06527hyod-17006 */
        }                                                      /* @06527hyod-17006 */
        if( LdPcRdVac( UPPER, S_OFF ) == OK )                  /* @06527hyod-17006 */
        {                                                      /* @06527hyod-17006 */
            LdPcWtVac ( UPPER, S_OFF );                        /* @06527hyod-17006 */
        }                                                      /* @06527hyod-17006 */
        if( LdPcRdVac( LOWER, S_OFF ) == OK )                  /* @06527hyod-17006 */
        {                                                      /* @06527hyod-17006 */
            LdPcWtVac ( LOWER, S_OFF );                        /* @06527hyod-17006 */
        }                                                      /* @06527hyod-17006 */
    }                                                          /* @06527hyod-17006 */
    else                                                           /* @12808ykor-29900 : P-F1 */
    {                                                              /* @12808ykor-29900 : P-F1 */
        /* Subchuck-Z仕様で標準Arm（!Bernoulli）仕様の場合 */      /* @12808ykor-29900 : P-F1 */
        if( ( subchuckZMtr == TRUE )                               /* @12808ykor-29900 : P-F1 */
         && ( bernolliArm == FALSE ) )                             /* @12808ykor-29900 : P-F1 */
        {                                                          /* @12808ykor-29900 : P-F1 */
            if( wafInfo == WAF_EXIST )                             /* @12808ykor-29900 : P-F1 */
            {                                                      /* @12808ykor-29900 : P-F1 */
                while( 1 )                                         /* @12808ykor-29900 : P-F1 */
                {                                                  /* @12808ykor-29900 : P-F1 */
                    LdSc2PcWafPtrAndSram( UPPER, &pcWafOnFlg );    /* @12808ykor-29900 : P-F1 */
                    LdSc2PcWafPtrAndSram( LOWER, &pcWafOnFlg );    /* @12808ykor-29900 : P-F1 */
                    if( pcWafOnFlg == 0 )                          /* @12808ykor-29900 : P-F1 */
                    {                                              /* @12808ykor-29900 : P-F1 */
                        errRtn = LdErrMsg( SC_WAF_EXIST_CHECK,     /* @12808ykor-29900 : P-F1 */
                                        ERR_LOADER_ACK_VACCHK, 0 );/* @12808ykor-29900 : P-F1 */
                        if( errRtn == ERR_RETRY_MSG )              /* @12808ykor-29900 : P-F1 */ 
                        {                                          /* @12808ykor-29900 : P-F1 */ 
                            errRtn = LdErrMsg( SC_WAF_BREAK_WARN,  /* @12808ykor-29900 : P-F1 */              
                                       ERR_LOADER_ACK_VACCHK2, 0 );/* @12808ykor-29900 : P-F1 */ 
                            if( errRtn != ERR_RETRY_MSG )          /* @12808ykor-29900 : P-F1 */ 
                            {                                      /* @12808ykor-29900 : P-F1 */ 
                                break ;                            /* @12808ykor-29900 : P-F1 */ 
                            }                                      /* @12808ykor-29900 : P-F1 */ 
                        }                                          /* @12808ykor-29900 : P-F1 */  
                    }                                              /* @12808ykor-29900 : P-F1 */
                    else                                           /* @12808ykor-29900 : P-F1 */
                    {                                              /* @12808ykor-29900 : P-F1 */
                        break;                                     /* @12808ykor-29900 : P-F1 */
                    }                                              /* @12808ykor-29900 : P-F1 */
                }                                                  /* @12808ykor-29900 : P-F1 */
                SramSet4( SckWaf, 0 );                             /* @12808ykor-29900 : P-F1 */
                LdSetScWaf( WAF_EMPTY );                           /* @12808ykor-29900 : P-F1 */
            }                                                      /* @12808ykor-29900 : P-F1 */
        }                                                          /* @12808ykor-29900 : P-F1 */
    }                                                              /* @12808ykor-29900 : P-F1 */

    MQSend( LdRootTask, "PC_ORG_END", WAIT_FOREVER, 0 ) ;       /* @05124hyod-13989 */

/* '95.08.02 11-line added by c.ogino 
  ピンセットをホームへ戻したら、シャッターを閉めます。
 */
	if ( ShtExist )
    {

    /* 念のため、ディレイを入れておく */
        taskDelay ( TmGetAuxTickS ( 1 ) );

        rtn = LdShtRdSol ( 1 ) ;
        if ( rtn == OK ) /* シャッターが開いている */
        {
            if( ( rtn = LdShtClsSens( 1 ) ) )                /* @9223kish-01 */
            {                                                /* @9223kish-01 */
                return( rtn );                               /* @9223kish-01 */
            }                                                /* @9223kish-01 */
        }
    }    

/*********************************************/
/* ピンセットQ がＩＤ位置ならどうする。 後日 */
/*********************************************/
E_RETRY2:
    if( ( rtn = LdPqGoOrg() ) )                              /* @9223kish-01 */
	{
        errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 );
        if ( errRtn == ERR_RETRY_MSG ) goto E_RETRY2;
        if ( errRtn == LD_DIAG_ERR ) return ( rtn );
	}
    if( ( rtn = LdPqGoHm() ) )                               /* @9223kish-01 */
	{
        errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 );
        if ( errRtn == ERR_RETRY_MSG ) goto E_RETRY2;
        if ( errRtn == LD_DIAG_ERR ) return ( rtn );
	}
/* ピンセットＱがイ二シャルできなければピンセットＺは動かない see chkPzMove */
/* ピンセットＺを動かす */
/* やはり、ファースト仕様ではコールしてはまずいか
	PZ_NOT_UNLOCK が発生
	ファーストか、セカンドかは、イニッシャル前にわかっていなければ
	都合が悪い
*/
/* LdPzUp (), LdPzMove ( 0 ) は、使用できない */
    if( ( rtn = chkPzMove() ) )                              /* @9223kish-01 */
    {                                                        /* @9223kish-01 */
        return( LdSetErr( rtn, 0 ) );                        /* @9223kish-01 */
    }                                                        /* @9223kish-01 */

    /* Prober checks 8 inch wafer saving sensor           @00516kish-4031 */
    /* to prevent Pz's and wafer's colliding.             @00516kish-4031 */
E_RETRY7:                                              /* @00516kish-4031 */
    if( ( rtn = LdIdxChkWSave( 8 ) ) )                 /* @00516kish-4031 */
    {                                                  /* @00516kish-4031 */
        errRtn =  LdErrMsg( rtn, ERR_LOADER_RETRY, 0 );/* @00516kish-4031 */
        if( errRtn == ERR_RETRY_MSG )                  /* @00516kish-4031 */
        {                                              /* @00516kish-4031 */
            goto E_RETRY7;                             /* @00516kish-4031 */
        }                                              /* @00516kish-4031 */
        if( errRtn == LD_DIAG_ERR )                    /* @00516kish-4031 */
        {                                              /* @00516kish-4031 */
            return( rtn );                             /* @00516kish-4031 */
        }                                              /* @00516kish-4031 */
    }                                                  /* @00516kish-4031 */

#if( P_OCTO )                                                     /* @08910ykor-23437 */
    if( LdGetSingleType() == 0 )                                  /* @08910ykor-23437 */
    {                                                             /* @08910ykor-23437 */
#endif                                                            /* @08910ykor-23437 */
/* エラーをチェックできない for first */
E_RETRY3:
		dWait = PzLk.dWait;
		intV  = PzLk.intV;
		cTime = PzLk.cTime;
    	rtn = DrvSens( LdPzWtUnlk,1,S_ON,LdPzRdUnlk,1,S_ON,dWait,intV,cTime,1  );
#if( P_OCTO )                                                     /* @08910ykor-23437 */
    }                                                             /* @08910ykor-23437 */
#endif                                                            /* @08910ykor-23437 */

    if( ( OptionInfoGet( PO_2NDLOADER ) == 1 )                    /* @08910ykor-23437 */
     || ( OptionInfoGet( PO_2ND1CST ) == 1 ) )                    /* @08910ykor-23437 */
    {                                                             /* @08910ykor-23437 */
        if( ( rtn = LdPzDnSens( 0 ) ) )                           /* @08910ykor-23437 */
        {                                                         /* @08910ykor-23437 */
            errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 );        /* @08910ykor-23437 */
            if ( errRtn == ERR_RETRY_MSG )                        /* @08910ykor-23437 */
            {                                                     /* @08910ykor-23437 */
                goto E_RETRY3;                                    /* @08910ykor-23437 */
            }                                                     /* @08910ykor-23437 */
            if ( errRtn == LD_DIAG_ERR )                          /* @08910ykor-23437 */
            {                                                     /* @08910ykor-23437 */
                return( rtn );                                    /* @08910ykor-23437 */
            }                                                     /* @08910ykor-23437 */
        }                                                         /* @08910ykor-23437 */
        LdPzWtEnd();                                              /* @08910ykor-23437 */
        LdPartDt.pzPos = PZ_DN;                                   /* @08910ykor-23437 */

        taskDelay( TmGetAuxTickMs(1000) );                        /* @08910ykor-23437 */

    }                                                             /* @08910ykor-23437 */

#if( P_OCTO )                                                     /* @08910ykor-23437 */
    if( LdGetSingleType() == 0 )                                  /* @08910ykor-23437 */
    {                                                             /* @08910ykor-23437 */
#endif                                                            /* @08910ykor-23437 */
    	if( ( rtn = LdPzUpSens( 0 ) ) )                          /* @9223kish-01 */
		{
        	errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 );
        	if ( errRtn == ERR_RETRY_MSG ) goto E_RETRY3;
        	if ( errRtn == LD_DIAG_ERR ) return ( rtn );
		}
    	if( ( rtn = LdPzLock( 0 ) ) )                            /* @9223kish-01 */
		{
        	errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 );
        	if ( errRtn == ERR_RETRY_MSG ) goto E_RETRY3;
        	if ( errRtn == LD_DIAG_ERR ) return ( rtn );
		}
    	LdPzWtEnd();
    	LdPartDt.pzPos = PZ_UP;
#if( P_OCTO )                                                     /* @08910ykor-23437 */
    }                                                             /* @08910ykor-23437 */
#endif                                                            /* @08910ykor-23437 */

/* @08110ykor-21935    if ( ( MtrRdStatus ( MTR_UPC, 2 ) & MTR2_ORG_BIT ) == 0 ) */
/* @08110ykor-21935                return ( LdSetErr ( IDX_UPC_NOT_HOME, 0 ) ); */
/* @08110ykor-21935    if ( ( MtrRdStatus ( MTR_LPC, 2 ) & MTR2_ORG_BIT ) == 0 ) */
/* @08110ykor-21935                return ( LdSetErr ( IDX_LPC_NOT_HOME, 0 )  ); */
    if( ( rtn = LdChkPcIdxMove() ) )                    /* @08110ykor-21935 */
    {                                                   /* @08110ykor-21935 */
/* @08910ykor-23437         return( rtn );                                 @* @08110ykor-21935 */
         return( LdSetErr( rtn, 0 ) );                            /* @08910ykor-23437 */
    }                                                   /* @08110ykor-21935 */

#if LD_SPEC_SMIF                                          /* @9c02mtog-625 */
    /* checking a smif-pod exist or not                      @9c02mtog-625 */
    memset( &rcvBufSmf[0], 0x00, sizeof(rcvBufSmf) );     /* @9c02mtog-625 */
    memset( &sndBufSmf[0], 0x00, sizeof(sndBufSmf) );     /* @9c02mtog-625 */
    strcpy( &sndBufSmf[0], "LIXSTAT,0" );                 /* @9c02mtog-625 */
    MQSend( IdxControlTaskID, &sndBufSmf[0],              /* @9c02mtog-625 */
                                      WAIT_FOREVER, 0 );  /* @9c02mtog-625 */
    MQReceive( &fTaskSmf, &tTaskSmf, &paraNumSmf,         /* @9c02mtog-625 */
               &rcvBufSmf[0], WAIT_FOREVER, &ackComSmf,   /* @9c02mtog-625 */
                                        &ackBufSmf[0] );  /* @9c02mtog-625 */
    if( rtn )                                             /* @9c02mtog-625 */
    {                                                     /* @9c02mtog-625 */
        setErr = SMIF_CONNECT_ERR;                        /* @9c02mtog-625 */
        return( LdSetErr(setErr, 0) );                    /* @9c02mtog-625 */
    }                                                     /* @9c02mtog-625 */

    if ( InxSt0.pip == 1 )          /* A pod exists          @9c02mtog-625 */
    {                                                     /* @9c02mtog-625 */
        SPodExist = EXIST ;                               /* @9c02mtog-625 */
        MapDt [ 0 ].exist = EXIST ;
    }                                                     /* @9c02mtog-625 */
    else                                                  /* @9c02mtog-625 */
    {                                                     /* @9c02mtog-625 */
        SPodExist = 0 ;                                   /* @9c02mtog-625 */
    }                                                     /* @9c02mtog-625 */

    LdPartDt.idxPos = 0;                                  /* @9c02mtog-625 */
    memset( &rcvBufSmf[0], 0x00, sizeof(rcvBufSmf) );     /* @9c02mtog-625 */
    memset( &sndBufSmf[0], 0x00, sizeof(sndBufSmf) );     /* @9c02mtog-625 */
    strcpy( &sndBufSmf[0], "LIXMVHM" );                   /* @9c02mtog-625 */
    MQSend( IdxControlTaskID, &sndBufSmf[0], WAIT_FOREVER,/* @9c02mtog-625 */
                                                      0 );/* @9c02mtog-625 */
    MQReceive( &fTaskSmf, &tTaskSmf, &paraNumSmf,         /* @9c02mtog-625 */
                &rcvBufSmf[0], WAIT_FOREVER, &ackComSmf,  /* @9c02mtog-625 */
                                         &ackBufSmf[0] ); /* @9c02mtog-625 */
    MQGetNo( &rcvBufSmf[0], &rtn, 2 );                    /* @9c02mtog-625 */
/* ポッドが無いときはホーム駆動やステージ駆動は”ポッド      @9c02mtog-625 */
/* 無し(HCACK=65)”でエラー扱いみたいです。ちなみにポッドが  @9c02mtog-625 */
/* 無いときは、LIXMVHMの返値は”１5”となるようです。        @9c02mtog-625 */
/*（と言うよりRCMDでｲﾝﾃﾞｸｻｰ駆動に関するものについては、全て  @9c02mtog-625 */
/* HCACK=65が返りそうなので、要注意です。）                  @9c02mtog-625 */
/* イニシャル時に、あらかじめポッドが無い時はエラーにする    @9c02mtog-625 */
/* べきではない                                              @9c02mtog-625 */
    if( ( rtn != 0 ) && ( rtn != 15 ) )                   /* @9c02mtog-625 */
    {                                                     /* @9c02mtog-625 */
        LdErrRstSmf( );                                   /* @9c02mtog-625 */
        if( (rtn == 10) || (rtn == 20) )                  /* @9c02mtog-625 */
        {                                                 /* @9c02mtog-625 */
            setErr = SMIF_CONNECT_ERR;                    /* @9c02mtog-625 */
        }                                                 /* @9c02mtog-625 */
        else                                              /* @9c02mtog-625 */
        {                                                 /* @9c02mtog-625 */
            setErr = SMIF_IDX_DRV_ERR;                    /* @9c02mtog-625 */
        }                                                 /* @9c02mtog-625 */
        return( LdSetErr(setErr, 0) );                    /* @9c02mtog-625 */
    }                                                     /* @9c02mtog-625 */
    LdPartDt.idxPos = IDX_HM;                             /* @9c02mtog-625 */
    LdSetErr( 0,0 );                                      /* @9c02mtog-625 */
#else                                                     /* @9c02mtog-625 */
	/* インデクサを動かすので、チルトします */
	if( OptionInfoGet( PO_2ND1CST ) )
	{
        if( ( rtn = LdCstTilt( 1, 0 ) ) )                    /* @9223kish-01 */
        {                                                    /* @9223kish-01 */
            return( rtn );                                   /* @9223kish-01 */
        }                                                    /* @9223kish-01 */
	}
	else
	{
        if( ( rtn = LdCstTilt( 0, 0 ) ) )                    /* @9223kish-01 */
        {                                                    /* @9223kish-01 */
            return( rtn );                                   /* @9223kish-01 */
        }                                                    /* @9223kish-01 */
	}

	if( OptionInfoGet( PO_2NDLOADER ) )
	{
        if ( (rtn = LdCstTilt ( 1, 0 )) ) return ( rtn );    /* @9223kish-01 */
	}

E_RETRY6:
/* インデクサ停止仕様 	Written by M.Togawa  '96.05.23 */
	if( (LdCstRdSet(0, 1) == OK) || (LdCstRdSet(1, 1) == OK) )
	MQSend( LdIdxStopTask, "CHK_CLP", WAIT_FOREVER, 0 ) ;
	rtn = LdIdxGoOrg();
/* インデクサ停止仕様 	Written by M.Togawa  '96.05.23 */
	MQSend( LdIdxStopTask, "END_CLP", WAIT_FOREVER, 0 ) ;
	if ( rtn )
	{
		if( rtn == (CST_NOT_CLAMP1 + 2000) )
		{
			LdCstRls( 0, 0 ) ;
		}
		else if( rtn == (CST_NOT_CLAMP2 + 2000) )
		{
			LdCstRls( 1, 0 ) ;
		}
		else	return( rtn ) ;

        errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 );
        if ( errRtn == ERR_RETRY_MSG )
		{
			if( rtn == (CST_NOT_CLAMP1 + 2000) )
			{
    			intV = CstRls [ 0 ].intV;
    			cTime= CstRls [ 0 ].cTime;

        		DrvSens( LdCstWtRls, 2, 0, S_OFF,
                       LdCstRdRls,2, 0, S_OFF, 0 , intV, cTime , 0 );
			}
			else if( rtn == (CST_NOT_CLAMP2 + 2000) )
			{
    			intV = CstRls [ 1 ].intV;
    			cTime= CstRls [ 1 ].cTime;

                DrvSens( LdCstWtRls, 2, 1, S_OFF,
                       LdCstRdRls,2, 1, S_OFF, 0 , intV, cTime , 0 );
			}
			goto E_RETRY6;
		}
        if ( errRtn == LD_DIAG_ERR )
		{
			 return ( rtn );
		}
	}

/* インデクサ停止仕様 	Written by M.Togawa  '96.05.23 */
	if( (LdCstRdSet(0, 1) == OK) || (LdCstRdSet(1, 1) == OK) )
	MQSend( LdIdxStopTask, "CHK_CLP", WAIT_FOREVER, 0 ) ;
	rtn = LdIdxGoHm();
/* インデクサ停止仕様 	Written by M.Togawa  '96.05.23 */
	MQSend( LdIdxStopTask, "END_CLP", WAIT_FOREVER, 0 ) ;
	if ( rtn )
	{
		if( rtn == (CST_NOT_CLAMP1 + 2000) )
		{
			LdCstRls( 0, 0 ) ;
		}
		else if( rtn == (CST_NOT_CLAMP2 + 2000) )
		{
			LdCstRls( 1, 0 ) ;
		}
		else	return( rtn ) ; 

       	errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 ) ;
       	if ( errRtn == ERR_RETRY_MSG )
		{
			if( rtn == (CST_NOT_CLAMP1 + 2000) )
			{
    			intV = CstRls [ 0 ].intV;
    			cTime= CstRls [ 0 ].cTime;

                DrvSens( LdCstWtRls, 2, 0, S_OFF,
                      LdCstRdRls,2, 0, S_OFF, 0 , intV, cTime , 0 );
			}
			else if( rtn == (CST_NOT_CLAMP2 + 2000) )
			{
    			intV = CstRls [ 1 ].intV;
    			cTime= CstRls [ 1 ].cTime;

                DrvSens( LdCstWtRls, 2, 1, S_OFF,
                      LdCstRdRls,2, 1, S_OFF, 0 , intV, cTime , 0 );
			}
			goto E_RETRY6;
		}
       	else if ( errRtn == LD_DIAG_ERR ) return ( rtn );
	}

	for ( cNo = 0; cNo < CST_MAX; cNo++ )
	{
		if ( LdCstRdSet ( cNo, S_ON ) == OK )
		{
			if ( LdCstRdRls ( cNo, S_OFF ) == OK )
			{
				LdLedWtClp ( cNo, S_ON );
	/* カセット有(1),無(0) */
				MapDt [ cNo ].exist = 1;
			}
			else
			{
				intV  = CstRls [ cNo ].intV;	
				cTime = CstRls [ cNo ].cTime;	
				LdCstWtRls ( cNo, S_ON );
				rtn = DrvSens ( LdCstWtRls, 2, cNo, S_OFF,
								LdCstRdRls, 2, cNo, S_OFF, 0, intV, cTime, 1 );
				if ( rtn )
				{
					rtn = DrvSens ( LdCstWtRls, 2, cNo, S_ON,
									LdCstRdRls, 2, cNo, S_ON, 0,intV, cTime, 0 );
					LdLedWtClp ( cNo, S_OFF );
	/* カセット有(1),無(0) */
					MapDt [ cNo ].exist = 0;
				}
				else
				{   
					LdLedWtClp ( cNo, S_ON );
	/* カセット有(1),無(0) */
					MapDt [ cNo ].exist = 1;
				}
			}
		}
		else
		{
			/* カセットがないので、リリースできるはず */
            if( ( rtn = LdCstRls( cNo, 0 ) ) )               /* @9223kish-01 */
            {                                                /* @9223kish-01 */
                return( rtn );                               /* @9223kish-01 */
            }                                                /* @9223kish-01 */
			LdLedWtClp ( cNo, S_OFF );
			MapDt [ cNo ].exist = 0;
		}
	}
#endif                                                    /* @9c02mtog-625 */
/* @06527hyod-17006    if( OptionInfoGet( PO_SUBCHUCKZ ))                     @* @00602kish-4053 */
/* @06527hyod-17006    {                                                      @* @00602kish-4053 */
/* @06527hyod-17006        if( ( rtn = LdScVacOff( 1 ) ) )                    @* @00602kish-4053 */
/* @06527hyod-17006        {                                                  @* @00602kish-4053 */
/* @06527hyod-17006            return( rtn );                                 @* @00602kish-4053 */
/* @06527hyod-17006        }                                                  @* @00602kish-4053 */
/* @06527hyod-17006E_RETRY8:                                                  @* @00602kish-4053 */
/* @06527hyod-17006        if( ( rtn = LdScZGoOrg() ) )                       @* @00602kish-4053 */
/* @06527hyod-17006        {                                                  @* @00602kish-4053 */
/* @06527hyod-17006            errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 ); @* @00602kish-4053 */
/* @06527hyod-17006            if ( errRtn == ERR_RETRY_MSG ) goto E_RETRY8;  @* @00602kish-4053 */
/* @06527hyod-17006            if ( errRtn == LD_DIAG_ERR ) return ( rtn );   @* @00602kish-4053 */
/* @06527hyod-17006        }                                                  @* @00602kish-4053 */
/* @06527hyod-17006        if( ( rtn = LdScZGoHm() ) )                        @* @00602kish-4053 */
/* @06527hyod-17006        {                                                  @* @00602kish-4053 */
/* @06527hyod-17006            errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 ); @* @00602kish-4053 */
/* @06527hyod-17006            if ( errRtn == ERR_RETRY_MSG ) goto E_RETRY8;  @* @00602kish-4053 */
/* @06527hyod-17006            if ( errRtn == LD_DIAG_ERR ) return ( rtn );   @* @00602kish-4053 */
/* @06527hyod-17006        }                                                  @* @00602kish-4053 */
/* @06527hyod-17006    }                                                      @* @00602kish-4053 */
	return ( OK );
}
/*#==================================================================#*/
/*#        Function    : LdWafReturn								 #*/
/*#        Summary     : return wafer 								 #*/
/*#        Argument    : 											 #*/
/*#						intl	fmId; from-ID						 #*/
/*#						intl	toId; to-ID							 #*/
/*#						intl	usePc; use pincette				 	 #*/
/*#						intl	selPaln; prealign select			 #*/
/*#						intl	ptrEWafDt; pointer of EWafDt		 #*/
/*#        Return      : error									     #*/	
/*#						output of LdPqGoHm							 #*/
/*#						output of LdMove						     #*/
/*#						output of LdIdxGoHm						     #*/
/*#        Extern      : tyMapDt MapDt [ cstNo ];					 #*/
/*#        Caution     : 											 #*/
/*#			本関数は、イニシャルあるいは、リセット時のウェーハ返却の #*/
/*#			ために使用します。										 #*/
/*#			また、LD_FUNC においても　ウェーハ移動において toId = 0  #*/
/*#			の処理で使用しています。								 #*/
/*#==================================================================#*/
intl    LdWafReturn ( fmId, toId, usePc, selPaln, ptrEWafDt )
    intl    fmId;
    intl    toId;
    intl    usePc;
    intl    selPaln;
    intl    ptrEWafDt;
{ 
/* @08612rame-22179    intl    cstNo;   */
    intl    cstNo = 0;             /* @08612rame-22179 */
    intl    rtn;
#if LD_SPEC_SMIF                          /* @01b20kish-7382 */
#else                                     /* @01b20kish-7382 */
    intl    errRtn;
	intl	intV;
	intl	cTime;
    intl    tmpSelPaln;             /* @9b16mima-2936 */
#endif                                    /* @01b20kish-7382 */
    intl    slotNo;                 /* @01411kish-5506 */

    intl    wafSize;                  /* @08612rame-22179 */

    if( LdGetProcs( ) == LD_INIT )  /* @7805mtog-00 */
    {                               /* @7805mtog-00 */
        wafRetOnInit = 1;           /* @7805mtog-00 */
    }                               /* @7805mtog-00 */

/* '95.06.19 7-lines added by c.ogino
   ウェハーがＩＤ位置の時、マスターからのfmIdが0x2000なので
  ピンセットθの位置で、ピンセットをホームに戻すか否かを判断します。 */
	if ( LD_INIT_DEBUG > 0 )
		logMsg ( " fmId = %d , PqPos = %d\n", fmId, LdPartDt.pqPos ) ;

	if ( ( fmId == LID ) || ( LdPartDt.pqPos == PQ_ID ) )
	{
        if( ( rtn = LdPcGoHm( usePc ) ) )                    /* @9223kish-01 */
        {                                                    /* @9223kish-01 */
            return( rtn ) ;                                  /* @9223kish-01 */
        }                                                    /* @9223kish-01 */
	}

/* fmId をホームに直す必要あり */
    if( ( rtn = LdPqGoHm() ) )                               /* @9223kish-01 */
    {                                                        /* @9223kish-01 */
        return( rtn );                                       /* @9223kish-01 */
    }                                                        /* @9223kish-01 */

    if ( toId & CST )
    {
        cstNo = LdGetCstNo ( toId );
        slotNo = LdGetSlotNo( toId );                        /* @18a24rsas-41241 */
        if( LdUnldPosChk( cstNo, usePc, slotNo ) == ERROR )  /* @22b17hlam-T24080 */
        {                                                    /* @22b17hlam-T24080 */
            toId = ChkRtnCalCstWaf( cstNo, slotNo );         /* @22b17hlam-T24080 */
        }                                                    /* @22b17hlam-T24080 */
/* @22b17hlam-T24080         if( LdUnldPosChk( cstNo, usePc, slotNo ) == OK )      @* @18a24rsas-41241 */
        if( toId != 0 )                                      /* @22b17hlam-T24080 */
        {                                                    /* @18a24rsas-41241 */
          if ( MapDt [cstNo].exist != 0)
          {
			if ( MapDt[cstNo].mapEnd != 0 )
			{
                if( SysState == SYS_WAFSORT )               /* @01411kish-5506 */
                {                                           /* @01411kish-5506 */
                    slotNo = LdGetSlotNo( toId );           /* @01411kish-5506 */
                    if( MapDt[cstNo].slot [slotNo].nowExist /* @01411kish-5506 */
                                  != WAF_EMPTY )            /* @01411kish-5506 */
                    {                                       /* @01411kish-5506 */
                        toId = LdGetWafReturnEmpty( cstNo );/* @01411kish-5506 */
                        if( toId == ERROR )                 /* @01411kish-5506 */
                        {                                   /* @01411kish-5506 */
                            if( fmId == W_TBL )             /* @01411kish-5506 */
                            {                               /* @01411kish-5506 */
                                return( OK );               /* @01411kish-5506 */
                            }                               /* @01411kish-5506 */
                            else                            /* @01411kish-5506 */
                            {                               /* @01411kish-5506 */
#if( LD_SPEC_SMIF )                                         /* @01b20kish-7382 */
#else                                                       /* @01b20kish-7382 */
                                goto WTBL_RTN;              /* @01411kish-5506 */
#endif                                                      /* @01b20kish-7382 */
                            }                               /* @01411kish-5506 */
                        }                                   /* @01411kish-5506 */
                    }                                       /* @01411kish-5506 */
                }                                           /* @01411kish-5506 */

				LdSetMoveWafDt (fmId,toId,usePc,0,selPaln,0,0,1,0,ptrEWafDt );
	/* set destPos in EWafDt */
				LdSetEWafDest ( ptrEWafDt, toId );
                if( ( rtn = LdMove() ) )                     /* @9223kish-01 */
                {                                            /* @9223kish-01 */
                    return( rtn );                           /* @9223kish-01 */
                }                                            /* @9223kish-01 */
	/* 位置の更新をします。EwafDt */
				LdSetEWafNPos ( ptrEWafDt, toId );
				LdSetEWafDest ( ptrEWafDt, 0 ); /* 移動終了 */
				return ( OK );
			}
			else
			{
				if ( LdGetLdOperate () )
				{
					LdSetMoveWafDt (fmId,toId,usePc,0,selPaln,0,0,1,0,ptrEWafDt );
	/* set destPos in EWafDt */
					LdSetEWafDest ( ptrEWafDt, toId );
                    if( ( rtn = LdMove() ) )                 /* @9223kish-01 */
                    {                                        /* @9223kish-01 */
                        return( rtn );                       /* @9223kish-01 */
                    }                                        /* @9223kish-01 */
	/* 位置の更新をします。EwafDt */
					LdSetEWafNPos ( ptrEWafDt, toId );
					LdSetEWafDest ( ptrEWafDt, 0 ); /* 移動終了 */
#if LD_SPEC_SMIF                                           /* @9c02mtog-625 */
                    if( (MtrRdStatus(MTR_UPC, 2) &         /* @9c02mtog-625 */
                                     MTR2_ORG_BIT) == 0 )  /* @9c02mtog-625 */
                    {                                      /* @9c02mtog-625 */
                        return( (LdSetErr(IDX_UPC_NOT_HOME,/* @9c02mtog-625 */
                                                     0)) );/* @9c02mtog-625 */
                    }                                      /* @9c02mtog-625 */
                    if( (MtrRdStatus(MTR_LPC, 2) &         /* @9c02mtog-625 */
                                     MTR2_ORG_BIT) == 0 )  /* @9c02mtog-625 */
                    {                                      /* @9c02mtog-625 */
                        return( (LdSetErr(IDX_LPC_NOT_HOME,/* @9c02mtog-625 */
                                                     0)) );/* @9c02mtog-625 */
                    }                                      /* @9c02mtog-625 */
                    LdPartDt.idxPos = 0;                   /* @9c02mtog-625 */
                    memset( &rcvBufSmf[0], 0x00,           /* @9c02mtog-625 */
                                      sizeof(rcvBufSmf) ); /* @8723mtog-SMIF */
                    memset( &sndBufSmf[0], 0x00,           /* @9c02mtog-625 */
                                      sizeof(sndBufSmf) ); /* @8723mtog-SMIF */
                    strcpy( &sndBufSmf[0], "LIXMVHM" );    /* @8723mtog-SMIF */
                    LdMsgSndSmif( &sndBufSmf[0], 2 );      /* @9b26kish-3146 */
                    LdPartDt.idxPos = IDX_HM;              /* @8723mtog-SMIF */
                    LdSetErr( 0,0 );                       /* @8723mtog-SMIF */
#endif                                                     /* @8723mtog-SMIF */
					return ( OK );
				}
			}
          }
        }                                                          /* @18a24rsas-41241 */
    }
    else                                                      /* @08612rame-22179 */
    {                                                         /* @08612rame-22179 */
/* @18b13rame-41195        if( OptionInfoGet( PO_BERNOULLI_ARM ) == 1 )          @* @08612rame-22179 */
        if( LdChkBerAirPress_Pc() == 1 )                      /* @18b13rame-41195 */
        {                                                     /* @08612rame-22179 */
            /* get a size of wafer from SRAM */
            wafSize = SramGet4( SizeOfWaf );                  /* @08612rame-22179 */

            /* wafer size check */
            if( (rtn = LdChkWafSize(wafSize)) )               /* @08612rame-22179 */
            {                                                 /* @08612rame-22179 */
                rtn = rtn - ERR_LOADER_TOP;                   /* @08612rame-22179 */
                LdErrMsg( rtn, ERR_SYSTEM_RESET,0 );          /* @08612rame-22179 */
            }                                                 /* @08612rame-22179 */

            if( (rtn = LdSetCstDt(cstNo, wafSize, -1 )) )     /* @08612rame-22179 */
            {                                                 /* @08612rame-22179 */
                rtn = rtn - ERR_LOADER_TOP;                   /* @08612rame-22179 */
                LdErrMsg( rtn, ERR_SYSTEM_RESET,0 );          /* @08612rame-22179 */
            }                                                 /* @08612rame-22179 */
        }                                                     /* @08612rame-22179 */
    }                                                         /* @08612rame-22179 */

#if LD_SPEC_SMIF                                           /* @9c02mtog-625 */
#else                                                      /* @9c02mtog-625 */
    if ( toId == B_TBL )
    {
        if ( IsNewWafPolish() && GetSPolishmode() )     /* @9b16mima-2936 */
        {                                               /* @9b16mima-2936 */
            tmpSelPaln = 0;                             /* @9b16mima-2936 */
        }                                               /* @9b16mima-2936 */
        else                                            /* @9b16mima-2936 */
        {                                               /* @9b16mima-2936 */
            tmpSelPaln = selPaln;                       /* @9b16mima-2936 */
        }                                               /* @9b16mima-2936 */
        LdSetMoveWafDt ( fmId, toId, usePc, 0, tmpSelPaln, 0, 0, 0, 0, ptrEWafDt ); /* @9b16mima-2936 */
        LdSetEWafDest ( ptrEWafDt, toId );
        if( ( rtn = LdMove() ) )                             /* @9223kish-01 */
        {                                                    /* @9223kish-01 */
            return( rtn );                                   /* @9223kish-01 */
        }                                                    /* @9223kish-01 */
/* 位置の更新をします。EwafDt */
        LdSetEWafNPos ( ptrEWafDt, toId );
        LdSetEWafDest ( ptrEWafDt, 0 ); /* 移動終了 */
        return ( OK );
    }
/*  パワーオンイニシャル時、ステージにあるウェーハを返却する場合も下の処理 */
WTBL_RTN:                                                    /* @01411kish-5506 */
E_RETRY:
    if ( LdWTblRdWaf ( S_OFF ) != OK )
    {
	/* インデクサ停止仕様 	Written by M.Togawa  '96.05.23 */
E_RETRY_IDXCLP:
		if( (LdCstRdSet(0, 1) == OK) || (LdCstRdSet(1, 1) == OK) )
		MQSend( LdIdxStopTask, "CHK_CLP", WAIT_FOREVER, 0 ) ;
		rtn = LdIdxGoHm ();
	/* インデクサ停止仕様 	Written by M.Togawa  '96.05.23 */
		MQSend( LdIdxStopTask, "END_CLP", WAIT_FOREVER, 0 ) ;
        if ( rtn )
		{
			if( rtn == CST_NOT_CLAMP1 + 2000 )
			{
				LdCstRls( 0, 0 ) ;
			}
			else if( rtn == CST_NOT_CLAMP2 + 2000 )
			{
				LdCstRls( 1, 0 ) ;
			}
        	errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 );
        	if ( errRtn == ERR_RETRY_MSG )
			{
				if( rtn == CST_NOT_CLAMP1 + 2000 )
				{
    				intV = CstRls [ 0 ].intV;
    				cTime= CstRls [ 0 ].cTime;

                   	DrvSens( LdCstWtRls, 2, 0, S_OFF,
                       		LdCstRdRls,2, 0, S_OFF, 0 , intV, cTime , 0 );
				}
				else if( rtn == CST_NOT_CLAMP2 + 2000 )
				{
    				intV = CstRls [ 1 ].intV;
    				cTime= CstRls [ 1 ].cTime;

                  	DrvSens( LdCstWtRls, 2, 1, S_OFF,
                      		LdCstRdRls,2, 1, S_OFF, 0 , intV, cTime , 0 );
				}
				goto E_RETRY_IDXCLP;
			}
		}
        errRtn = LdErrMsg ( WTBL_WAFER_EXIST, ERR_LOADER_RETRY,0 );
        if ( errRtn == ERR_RETRY_MSG ) goto E_RETRY;
    }
    else
    {
/*
ウェーハテーブルが空いていたらウェーハテーブルにセット
*/
        LdSetWTblWaf ( WAF_EMPTY );
        LdSetMoveWafDt ( fmId, W_TBL, usePc, 0, selPaln, 0, 0, 0, 0, ptrEWafDt);
/* set destPos in EWafDt */
        LdSetEWafDest ( ptrEWafDt, W_TBL );
        if( ( rtn = LdMove() ) )                             /* @9223kish-01 */
        {                                                    /* @9223kish-01 */
            return( rtn );                                   /* @9223kish-01 */
        }                                                    /* @9223kish-01 */
/* 位置の更新をします。EwafDt */
        LdSetEWafNPos ( ptrEWafDt, W_TBL );
        LdSetEWafDest ( ptrEWafDt, 0 ); /* 移動終了 */
        if( userNoGet() == IBM )                                     /* @00906kish-4735 */
        {                                                            /* @00906kish-4735 */
            if( toId != W_TBL )                                      /* @00906kish-4735 */
            {                                                        /* @00906kish-4735 */
E_RETRY_IDXCLP2:                                                     /* @00906kish-4735 */
            /* インデクサ停止仕様  */                                /* @00906kish-4735 */
                if((LdCstRdSet(0, 1) == OK) || (LdCstRdSet(1, 1) == OK)) /* @00906kish-4735 */
                {                                                    /* @00906kish-4735 */
                    MQSend( LdIdxStopTask, "CHK_CLP", WAIT_FOREVER, 0 ); /* @00906kish-4735 */
                }                                                    /* @00906kish-4735 */
                rtn = LdIdxGoHm();                                   /* @00906kish-4735 */
            /* インデクサ停止仕様  */                                /* @00906kish-4735 */
                MQSend( LdIdxStopTask, "END_CLP", WAIT_FOREVER, 0 ); /* @00906kish-4735 */
                if( rtn )                                            /* @00906kish-4735 */
                {                                                    /* @00906kish-4735 */
                    if( (rtn==(CST_NOT_CLAMP1 + 2000)) ||            /* @00906kish-4735 */
                                 (rtn==(CST_NOT_CLAMP1 + 2000)) )    /* @00906kish-4735 */
                    {                                                /* @00906kish-4735 */
                    /* インデクサ停止エラー処理 */                   /* @00906kish-4735 */
                        rtn = LdIdxStopErr( rtn );                   /* @00906kish-4735 */
                        goto E_RETRY_IDXCLP2;                        /* @00906kish-4735 */
                    }                                                /* @00906kish-4735 */
                    else                                             /* @00906kish-4735 */
                    {                                                /* @00906kish-4735 */
                        return( rtn );                               /* @00906kish-4735 */
                    }                                                /* @00906kish-4735 */
                }                                                    /* @00906kish-4735 */
                LdErrMsg( LD_WAFRTN_TO_WTBL, ERR_LOADER_ACK, 0 );    /* @00906kish-4735 */
                LdSetWTblWaf( WAF_EMPTY );                           /* @00906kish-4735 */
                LdClrWTblDt();                                       /* @00906kish-4735 */
            }                                                        /* @00906kish-4735 */
        }                                                            /* @00906kish-4735 */
        return ( OK );
    }
#endif                                                     /* @9c02mtog-625 */
    return ( OK );
}

#if LD_SPEC_SMIF                                           /* @9c02mtog-625 */
/*#------------------------------------------------------------------#*/
/*#        Function    : LdWafRtnSmif                                #*/
/*#        Summary     : #*/
/*#        Argument    : #*/
/*#        Return      : #*/
/*#        Extern      : #*/
/*#        Caution     : #*/
/*#------------------------------------------------------------------#*/
intl LdWafRtnSmif( intl usePc, intl ptr )
{
    intl rtn      = 0;
    intl errRtn   = 0;
    intl foundFlg = 0;
/* @01b20kish-7382    intl sN;     */
/* @01b20kish-7382    intl fT, tT; */
    intl fmId;
    intl toId=(-1);
/* @01b20kish-7382    intl mNum;   */
    intl mvslAddrLPc, nowSlot;
    intl umAddr;
    intl milAddr;
    intl setErr=0;
    intl wafOption;
#if LD_SPEC_SMIF                          /* @01b20kish-7382 */
#else                                     /* @01b20kish-7382 */
    intl sN;                              /* @01b20kish-7382 */
    intl fT, tT;                          /* @01b20kish-7382 */
    intl mNum;                            /* @01b20kish-7382 */
#endif                                    /* @01b20kish-7382 */

    fmId = HOME;

    if( LdGetProcs() == LD_INIT )
    {
        wafRetOnInit = 1;
    }

    if( MapDt[0].mapEnd != 0 )
    {
        for( foundFlg=0; foundFlg < SYS_SLOT_MAX; foundFlg++ )
        {
            if( MapDt[0].slot[foundFlg].nowExist == WAF_EMPTY )
            {
                toId = 0x80 + foundFlg;
                break;
            }
        }
    }

    if( foundFlg == SYS_SLOT_MAX )
    {
        LdPartDt.idxPos = 0;
        memset( &rcvBufSmf[0], 0x00, sizeof(rcvBufSmf) );
        memset( &sndBufSmf[0], 0x00, sizeof(sndBufSmf) );
        strcpy( &sndBufSmf[0], "LIXMVHM" );
        LdMsgSndSmif( &sndBufSmf[0], 2 );
        errRtn = LdErrMsg( SMIF_NONE_EMP_SLT, ERR_LOADER_ACK, 0 );
    }
    LdSetErr( 0,0 );
    LdPartDt.idxPos = IDX_HM;

/* @01b20kish-7382    if( rtn = LdPqGoHm( ) ) */
    if(( rtn = LdPqGoHm( ) ))                  /* @01b20kish-7382 */
    {
        return( rtn );
    }

RETRY_EMPSL_SMIF:
    LdPartDt.idxPos = 0;
    memset( &rcvBufSmf[0], 0x00, sizeof(rcvBufSmf) );
    memset( &sndBufSmf[0], 0x00, sizeof(sndBufSmf) );
    strcpy( &sndBufSmf[0], "LIXMVEMP,0" );
    MQSend( IdxControlTaskID, &sndBufSmf[0], WAIT_FOREVER, 0 );
    MQReceive( &fTaskSmf, &tTaskSmf, &paraNumSmf, &rcvBufSmf[0],
                        WAIT_FOREVER, &ackComSmf, &ackBufSmf[0] );
    MQGetNo( &rcvBufSmf[0], &rtn, 3 );
    if( rtn )
    {
        if( (rtn == 10) || (rtn == 20) )
        {
            setErr = SMIF_CONNECT_ERR;
            errRtn = LdErrMsg( setErr, ERR_SYSTEM_RESET, 0 );
        }
        else
        {
            LdPartDt.idxPos = 0;
            memset( &rcvBufSmf[0], 0x00, sizeof(rcvBufSmf) );
            memset( &sndBufSmf[0], 0x00, sizeof(sndBufSmf) );
            strcpy( &sndBufSmf[0], "LIXMVHM" );
            LdMsgSndSmif( &sndBufSmf[0], 2 );
            LdSetErr( 0,0 );
            LdPartDt.idxPos = IDX_HM;

            setErr = SMIF_DRV_NONE_EMPSLT;
            errRtn = LdErrMsg( setErr, ERR_LOADER_ACK, 0 );
            if( errRtn == ERR_MSG_MSG )
            {
                goto RETRY_EMPSL_SMIF;
            }
        }
    }
    LdPartDt.idxPos    = IDX_CST1;
    LdPartDt.idxSubPos = IDX_LDACSP;
    LdPartDt.idxUsePc  = usePc;

/* Get an address of the IDX */
    rtn = LdGetIdxAddr( &umAddr, &milAddr );
    if( rtn )
    {
        errRtn = LdErrMsg( SMIF_CONNECT_ERR, ERR_SYSTEM_RESET, 0 );
    }
    nowSlot = InxSt0.sltPos;
    LdPartDt.idxSlot = nowSlot;
    if( usePc == LOWER )
    {
        mvslAddrLPc = InxSt0.elPos + UPC_LPC_DEST;
        LdPartDt.idxPos = 0;
        memset( &rcvBufSmf[0], 0x00, sizeof(rcvBufSmf) );
        memset( &sndBufSmf[0], 0x00, sizeof(sndBufSmf) );
        sprintf( &sndBufSmf[0], "LIXMVAD,%d", mvslAddrLPc );
        LdMsgSndSmif( &sndBufSmf[0], 3 );
        LdSetErr( 0,0 );
    }

    LdSetMoveWafDt( fmId, toId, usePc, 0, 0, 0, 0, 1, 0, ptr );
    LdSetMoveMngDt( fmId, toId, usePc, 0, 0, 0, 0, 1, 0, ptr );
    LdSetEWafDest( ptr, toId );

    rtn = ldPc2CstSmif( usePc, nowSlot, mvslAddrLPc );
    if( rtn )
    {
        return( rtn );
    }

    LdSetPcWaf ( usePc, WAF_EMPTY );
    LdSetCstWaf ( 0, nowSlot, ptr );

    LdSetEWafNPos( ptr, toId );
    LdSetEWafDest( ptr, 0 );
    if( (MtrRdStatus(MTR_UPC, 2) & MTR2_ORG_BIT) == 0 )
    {
        return( (LdSetErr(IDX_UPC_NOT_HOME, 0)) );
    }
    if( (MtrRdStatus(MTR_LPC, 2) & MTR2_ORG_BIT) == 0 )
    {
        return( (LdSetErr(IDX_LPC_NOT_HOME, 0)) );
    }
/* IDX go home SMIF */
    LdPartDt.idxPos = 0;
    memset( &rcvBufSmf[0], 0x00, sizeof(rcvBufSmf) );
    memset( &sndBufSmf[0], 0x00, sizeof(sndBufSmf) );
    strcpy( &sndBufSmf[0], "LIXMVHM" );
    LdMsgSndSmif( &sndBufSmf[0], 2 );
    LdSetErr( 0,0 );
    LdPartDt.idxPos = IDX_HM;

    LdGetWafDt( &WafSize, &wafOption );

    return( OK );
}
/*#------------------------------------------------------------------#*/
/*#        Function    : ldPc2CstSmif                          #*/
/*#        Summary     : #*/
/*#        Argument    : #*/
/*#        Return      : #*/
/*#        Extern      : #*/
/*#        Caution     : #*/
/*#------------------------------------------------------------------#*/
intl ldPc2CstSmif( intl usePc, intl rtnSlot, intl errAddr )
{
    intl rtn = 0;
    intl errRtn;
    intl dWait, intV, cTime;

/* @01b20kish-7382    if( rtn = LdPzMove(0) ) */
    if(( rtn = LdPzMove(0) ))                       /* @01b20kish-7382 */
    {
        return( rtn );
    }

/* @01b20kish-7382    if( rtn = LdPcVacOn(usePc,1) ) */
    if(( rtn = LdPcVacOn(usePc,1) ))                /* @01b20kish-7382 */
    {
        return( rtn );
    }

/* @01b20kish-7382    if( rtn = LdScGoDn(usePc) ) */
    if(( rtn = LdScGoDn(usePc) ))                   /* @01b20kish-7382 */
    {
        return( rtn );
    }

RETRY_PCST_SMIF:
    rtn = LdPcGoInCst( 0, usePc ) ;
    if ( rtn )
    {
        errRtn = LdErrMsg( rtn, ERR_LOADER_RETRY, 0 );
        if( errRtn == ERR_RETRY_MSG )
        {
RETRY_PORG_SMIF:
/* @01b20kish-7382            if( rtn = LdPcGoOrg(usePc) ) */
            if(( rtn = LdPcGoOrg(usePc) ))         /* @01b20kish-7382 */
            {
                errRtn = LdErrMsg( rtn, ERR_LOADER_RETRY, 0 );
                if( errRtn == ERR_RETRY_MSG )
                {
                    goto RETRY_PORG_SMIF;
                }
            }
            else
            {
                goto RETRY_PCST_SMIF;
            }
        }
    }
    LdPcWtVac( usePc,  S_OFF );

RETRY_PCVAC:
    LdPartDt.idxPos = 0;
    dircSmf = 1;
    memset( &rcvBufSmf[0], 0x00, sizeof(rcvBufSmf) );
    memset( &sndBufSmf[0], 0x00, sizeof(sndBufSmf) );
    sprintf( &sndBufSmf[0], "LIXMVMI,%d,%d,%d", usePc, rtnSlot, dircSmf );
    MQSend( IdxControlTaskID, &sndBufSmf[0], WAIT_FOREVER, 0 );
    MQReceive( &fTaskSmf, &tTaskSmf, &paraNumSmf, &rcvBufSmf[0],
                           WAIT_FOREVER, &ackComSmf, &ackBufSmf[0] );
    MQGetNo( &rcvBufSmf[0], &rtn, 5 );
    LdPcWtVac ( usePc, S_OFF );
    if( rtn )
    {
        LdPartDt.idxPos = 0;
        loadUnload = 1;
        memset( &rcvBufSmf[0], 0x00, sizeof(rcvBufSmf) );
        memset( &sndBufSmf[0], 0x00, sizeof(sndBufSmf) );
        sprintf( &sndBufSmf[0], "LIXMVAD,%d", errAddr );
        LdMsgSndSmif( &sndBufSmf[0], 3 );
    }
    LdSetErr( 0,0 );
    LdPartDt.idxPos    = IDX_CST1;
    LdPartDt.idxSubPos = IDX_UDP;
    LdPartDt.idxUsePc  = usePc;
    LdPartDt.idxSlot   = --rtnSlot;

    dWait= PcVac [ usePc ].dWait;
    intV = PcVac [ usePc ].intV;
    cTime= PcVac [ usePc ].cTime;

    rtn =DrvSens(LdPcWtVac,2,usePc,S_OFF,
                 LdPcRdVac,2,usePc,S_OFF, 0, intV , cTime , 0 );
    if( rtn )
    {
        LdPartDt.idxPos = 0;
        loadUnload = 1;
        memset( &rcvBufSmf[0], 0x00, sizeof(rcvBufSmf) );
        memset( &sndBufSmf[0], 0x00, sizeof(sndBufSmf) );
        sprintf( &sndBufSmf[0], "LIXMVAD,%d", errAddr );
        LdMsgSndSmif( &sndBufSmf[0], 3 );

        LdSetErr( 0,0 );
        LdPartDt.idxPos    = IDX_CST1;
        LdPartDt.idxSubPos = IDX_UDACSP;
        LdPartDt.idxUsePc  = usePc;
        LdPartDt.idxSlot   = rtnSlot;
        errRtn = LdErrMsg ( PC_VAC_NOT_ON, ERR_LOADER_RETRY, 0 );
        if ( errRtn == ERR_RETRY_MSG )
        {
            goto RETRY_PCVAC;
        }
    }

RETRY_PCHM_SMIF:
/* @01b20kish-7382    if( rtn = LdPcGoHm(usePc) ) */
    if(( rtn = LdPcGoHm(usePc) ))                   /* @01b20kish-7382 */
    {
        errRtn = LdErrMsg( rtn, ERR_LOADER_RETRY, 0 );
        if( errRtn == ERR_RETRY_MSG )
        {
            goto RETRY_PCHM_SMIF;
        }
    }
    return( rtn );
}
#endif                                                     /* @9c02mtog-625 */

/* @01411kish-5506 */
/*#==================================================================#*/
/*#        Function    : LdGetWafReturnEmpty                         #*/
/*#        Summary     : get cassette empty return-position of wafer #*/
/*#        Argument    : intl   ptr; pointer of wafer                #*/
/*#        Return      :                                             #*/
/*#                     intl    cstNo;                               #*/
/*#                             ( CST ) (defined in LD.h)            #*/
/*#                     ERROR                                        #*/
/*#        Extern      :                                             #*/
/*#        Caution     :                                             #*/
/*#==================================================================#*/
intl LdGetWafReturnEmpty( cstNo )
    intl    cstNo;
{
    intl    slotMax;
    intl    sN;
    intl    rtn = -1;

    slotMax = chkMaxSlot( WafDt.size );

    for( sN = 0; sN < slotMax; sN++ )
    {
        if( MapDt[cstNo].slot[sN].nowExist == WAF_EMPTY )
        {
            rtn = LdGetMoveId( cstNo, sN );
            break;
        }
    }
    return( rtn );
}

/* @06323mtog-16644 */
/*#==================================================================#*/
/*#        Function    : LdReturnScWaf_Brnl                          #*/
/*#        Summary     : check if waf exist or not on Sc.            #*/
/*#        Argument    : none                                        #*/
/*#        Return      :                                             #*/
/*#        Extern      :                                             #*/
/*#        Caution     :                                             #*/
/*#==================================================================#*/
/* @09526rame-23042 intl LdReturnScWaf_Brnl( intl scExistErr ) */
intl LdReturnScWaf_Brnl( intl scExistErr, intl wafInfo )    /* @09526rame-23042 */
{
/* @09526rame-23042    intl    rtn;     */
/* @09526rame-23042    intl    errRtn;  */
    intl    rtn = OK;                                   /* @09526rame-23042 */
    intl    wafMoveFlag = FLAG_OFF;                     /* @09526rame-23042 */
    intl    wafStatus;                                  /* @09526rame-23042 */

    if( (rtn = LdScVacOff(0)) )
    {
        return( rtn );
    }
/* @08110rame-21925    LdPcWtVac( UPPER, S_ON );                       */

/* @09526rame-23042 E_RETRY_SCZ_INIT:                                  */
/* @09526rame-23042    rtn = LdScZGoOrg();                             */
/* @09526rame-23042	if( rtn == OK )                                    */
/* @09526rame-23042	{                                                  */
/* @09526rame-23042	    rtn = LdScZGoHm();                             */
/* @09526rame-23042	}                                                  */

/* @09526rame-23042	if( rtn != OK )                                    */
/* @09526rame-23042	{                                                  */
/* @09526rame-23042		errRtn = LdErrMsg( rtn, ERR_LOADER_RETRY,0 );  */
/* @09526rame-23042		if( errRtn == ERR_RETRY_MSG )                  */
/* @09526rame-23042		{                                              */
/* @09526rame-23042			goto E_RETRY_SCZ_INIT;                     */
/* @09526rame-23042		}                                              */

/* @09526rame-23042		if( errRtn == LD_DIAG_ERR )                    */
/* @09526rame-23042		{                                              */
/* @09526rame-23042			return( rtn );                             */
/* @09526rame-23042		}                                              */
/* @09526rame-23042	}                                                  */

    wafStatus = LdChkWafPalnLED( 4 );                                 /* @09526rame-23042 */

    if( wafStatus == WAF_EXIST )                                      /* @09526rame-23042 */
    {                                                                 /* @09526rame-23042 */
        LdScWtVac( S_ON );
        taskDelay( TmGetAuxTickMs(110) );

/* @08110rame-21925    if( (LdPcRdVac(UPPER, S_ON) != OK) && (LdScRdVac(S_ON) != OK) ) */
        if( LdScRdVac(S_ON) != OK)                       /* @08110rame-21925 */
        {
            if( LdPcRdVac( LOWER, S_ON ) != OK )                      /* @09526rame-23042 */
            {                                                         /* @09526rame-23042 */
                rtn = LdScZWafChk_Sc2Pc( LOWER );                     /* @09526rame-23042 */

                if( rtn == ERROR )                                    /* @09526rame-23042 */
                {                                                     /* @09526rame-23042 */
                    if( LdPcRdVac( UPPER, S_ON ) != OK )              /* @09526rame-23042 */
                    {                                                 /* @09526rame-23042 */
                        rtn = LdScZWafChk_Sc2Pc( UPPER );             /* @09526rame-23042 */

                        if( rtn == ERROR )                            /* @09526rame-23042 */
                        {                                             /* @09526rame-23042 */
/* @08110rame-21925        LdPcWtVac( UPPER, S_OFF ); */
/* @09526rame-23042                            LdScWtVac( S_OFF ) ; */
/* @09526rame-23042 PLS_WAF_REMOVE:                                 */
/* @09526rame-23042                            errRtn = LdErrMsg( REMOVE_SC_WAF, ERR_LOADER_ACK,0 ); */
/* @09526rame-23042                            goto PLS_WAF_REMOVE; */
                            LdRemoveWaf_Hand();                       /* @09526rame-23042 */
                        }                                             /* @09526rame-23042 */
                    }                                                 /* @09526rame-23042 */
                    else                                              /* @09526rame-23042 */
                    {                                                 /* @09526rame-23042 */
                        rtn = LdChkWafMove_Pc2Sc( UpcWaf );           /* @09526rame-23042 */

                        if( rtn == ERROR )                            /* @09526rame-23042 */
                        {                                             /* @09526rame-23042 */
                            wafMoveFlag = FLAG_ON;                    /* @09526rame-23042 */
                            rtn = OK;                                 /* @09526rame-23042 */
                        }                                             /* @09526rame-23042 */
                    }                                                 /* @09526rame-23042 */
                }                                                     /* @09526rame-23042 */
            }                                                         /* @09526rame-23042 */
            else                                                      /* @09526rame-23042 */
            {                                                         /* @09526rame-23042 */
                rtn = LdChkWafMove_Pc2Sc( LpcWaf );                   /* @09526rame-23042 */

                if( rtn == ERROR )                                    /* @09526rame-23042 */
                {                                                     /* @09526rame-23042 */
                    wafMoveFlag = FLAG_ON;                            /* @09526rame-23042 */
                    rtn = OK;                                         /* @09526rame-23042 */
                }                                                     /* @09526rame-23042 */
            }                                                         /* @09526rame-23042 */

            if( wafMoveFlag == FLAG_ON )                              /* @09526rame-23042 */
            {                                                         /* @09526rame-23042 */
                if( wafInfo == WAF_EXIST )                            /* @09526rame-23042 */
                {                                                     /* @09526rame-23042 */
                    LdRemoveWaf_Hand();                               /* @09526rame-23042 */
                }                                                     /* @09526rame-23042 */
            }                                                         /* @09526rame-23042 */
        }
/* @08110rame-21925    else if( LdScRdVac(S_ON) == OK ) */
        else                                             /* @08110rame-21925 */
        {
/* @08110rame-21925        LdPcWtVac( UPPER, S_OFF ); */
            if( scExistErr == SC_WAF_NOT_RETURN )
            {
                LdSetScWaf( WTBL_WAF );
            }
        }
    }                                                         /* @09526rame-23042 */
/* @09526rame-23042    return( OK );  */
    return( rtn );                                            /* @09526rame-23042 */
}

/* @06c13mtog-17805 */
#if ( !LD_SPEC_SMIF )      /* @06c13mtog-17805 */
/*#==================================================================#*/
/*#        Function    : LdMapWafReturn                              #*/
/*#        Summary     : map for return wafer                        #*/
/*#        Argument    : intl sramPtr; saved wafer pointer (on SRAM) #*/
/*#                      intl cNo;     casette number                #*/
/*#        Return      : toId                                        #*/
/*#        Extern      : tyMapDt MapDt [ cstNo ];                    #*/
/*#        Caution     :                                             #*/
/*#         本関数は、イニシャルあるいは、リセット時のウェーハ返却の #*/
/*#         ために使用します。                                       #*/
/*#==================================================================#*/
intl LdMapWafReturn( intl sramPtr, intl cNo )
{
    intl rtn;
    intl rtnVal, errRtn;
    intl wafSize;
    intl cstType;
    intl mapSlot;
    intl intV,cTime;
    intl toId=0;
    intl errSave;
    intl speedSel, sN, wPtr, cstPos;

    rtn = LdPcGoHm( UPPER );
    if( rtn )
    {
        LdErrMsg( rtn, ERR_SYSTEM_RESET,0 );
    }

    rtn = LdPcGoHm( LOWER );
    if( rtn )
    {
        LdErrMsg( rtn, ERR_SYSTEM_RESET,0 );
    }

    rtn = LdPqGoHm( );
    if( rtn )
    {
        LdErrMsg( rtn, ERR_SYSTEM_RESET,0 );
    }

    if( cNo == 1 )
    {
        sramPtr = sramPtr - SYS_SLOT_MAX;
    }

    if( MapDt[cNo].mapEnd == 0 )
    {
        if(( chkBernoulliArm() == 1 )                       /* @12426rame-29489 */
#if( P_OCTO )                                                   /* @12703rame-29272 */
        || ( WarpedLoader == TRUE )                             /* @12703rame-29272 */
#endif                                                          /* @12703rame-29272 */
        )                                                   /* @12426rame-29489 */
        {                                                   /* @12426rame-29489 */
            /* Map採取せず、返却ｳｪｰﾊはWTBLへ返却にします。     @12426rame-29489 */
            toId = 0;                                       /* @12426rame-29489 */
            return( toId );                                 /* @12426rame-29489 */
        }                                                   /* @12426rame-29489 */
        else                                                /* @12426rame-29489 */
        {                                                   /* @12426rame-29489 */
 
/* get a size of wafer from SRAM */
            wafSize = SramGet4( SizeOfWaf ) ;

            if( cNo == 0 )
            {
                if( LdCstRdSet ( cNo, S_ON ) != OK )
                {
                    return( toId );
                }
            }
            if( cNo == 1 )
            {
                if( LdCstRdSet ( cNo, S_ON ) != OK )
                {
                    return( toId );
                }
            }

/* wafer size check */
            if( (rtn = LdChkWafSize(wafSize)) )
            {
                rtn = rtn - ERR_LOADER_TOP;
                LdErrMsg( rtn, ERR_SYSTEM_RESET,0 );
            }

            LdGetCstType( wafSize, &cstType );

            if( (rtn = LdSetCstDt(cNo, wafSize, cstType)) )
            {
                rtn = rtn - ERR_LOADER_TOP;
                LdErrMsg( rtn, ERR_SYSTEM_RESET,0 );
            }

            LdSetWafDt( wafSize, 0 );
            mapSlot = CstDt[cNo].slotNum;


RETRY1_0:     /* RETRY RETRY */
            speedSel = 0;
RETRY1_1:     /* @05804yoda-15773 */
            if( cNo == 0 )
            {
                if( OptionInfoGet(PO_2NDLOADER) )
                {
                    LdCstTilt( 1, 0 );
                }
            }
            if( cNo == 1 )
            {
                if( OptionInfoGet( PO_2ND1CST ) == 0 )
                {
                    LdCstTilt( 0, 0 );
                }
            }

/* don't use the message "MOVE...." because this function may */
/* be called by LD_FUNC                                       */
RETRY_GETMAP:
            rtnVal = LdGetMap( speedSel, MapWk, cNo, wafSize, mapSlot );
            if( rtnVal )
            {
                if( rtnVal == 2955 )
                {
                    LdCstRls( 0, 0 );
                    errRtn = LdErrMsg( CST_NOT_CLAMP1, ERR_LOADER_RETRY,0 );
                    if( errRtn == ERR_RETRY_MSG )
                    {
RETRY_CST0_0:
                        if( LdCstRdSet(0, 1) )
                        {
                            errRtn = LdErrMsg( CST_NOT_EXIST1,
                                                        ERR_LOADER_RETRY, 0 );
                            if( errRtn == ERR_RETRY_MSG )
                            {
                                goto RETRY_CST0_0;
                            }
                        }
                        intV  = CstRls[0].intV;
                        cTime = CstRls[0].cTime;
                        DrvSens( LdCstWtRls, 2, 0, S_OFF,
                        LdCstRdRls,2,0,S_OFF, 0,intV,cTime,0 );
                        goto RETRY_GETMAP;
                    }
                }
                if( rtnVal == 2956 )
                {
                    LdCstRls( 1, 0 );
                    errRtn = LdErrMsg( CST_NOT_CLAMP2, ERR_LOADER_RETRY,0 );
                    if( errRtn == ERR_RETRY_MSG )
                    {
RETRY_CST0_1:
                        if( LdCstRdSet(1, 1) )
                        {
                            errRtn = LdErrMsg( CST_NOT_EXIST2,
                                                        ERR_LOADER_RETRY, 0 );
                            if( errRtn == ERR_RETRY_MSG )
                            {
                                goto RETRY_CST0_1;
                            }
                        }
                        intV  = CstRls[1].intV;
                        cTime = CstRls[1].cTime;
                        DrvSens( LdCstWtRls, 2, 1, S_OFF,
                                        LdCstRdRls,2,1,S_OFF, 0,intV,cTime,0 );
                        goto RETRY_GETMAP;
                    }
                }
                errSave = LdGetLocErr();
                if( mapErrCnt )
                {
                    mapErrCnt = 0;
                    toId      = 0;
                    return( toId );
                }
                if( errSave == INT_COUNT_NONE )
                {
                    mapErrCnt = LdGetMapDtFmSram( cNo, mapSlot, wafSize );
                    if( mapErrCnt )
                    {
                        errRtn = LdErrMsg( mapErrCnt, ERR_LOADER_RETRY, 0 );
                        if( errRtn == ERR_RETRY_MSG )
                        {
                            goto RETRY_GETMAP;
                        }
                    }
                }
                else
                {
                    if( chkIdxMapErr(errSave) ||
                        ( ( speedSel >= 1 ) && chkMapErr( errSave ) ) )
                    {
RETRY_STP2:
                    /* インデクサ停止仕様 */
                        if( (LdCstRdSet(0, 1) == OK) || (LdCstRdSet(1, 1) == OK) )
                        {
                            MQSend( LdIdxStopTask, "CHK_CLP", WAIT_FOREVER, 0 );
                        }
                        rtn = LdIdxGoHm();
                    /* インデクサ停止仕様 */
                        MQSend( LdIdxStopTask, "END_CLP", WAIT_FOREVER, 0 );
                        if ( rtn )
                        {
                            if( rtn == (CST_NOT_CLAMP1 + 2000) )
                            {
                                LdCstRls( 0, 0 ) ;
                                errRtn = LdErrMsg( rtn, ERR_LOADER_RETRY, 0 );
                            /* リトライ処理 */
                                if( errRtn == ERR_RETRY_MSG )
                                {
RETRY_CST1_1:
                                /* カセットが正常にセットされていますか */
                                    if( LdCstRdSet(0, 1) )
                                    {
                                        errRtn = LdErrMsg( CST_NOT_EXIST1,
                                                       ERR_LOADER_RETRY, 0 );
                                        if( errRtn == ERR_RETRY_MSG )
                                        {
                                            goto RETRY_CST1_1;
                                        }
                                    }

                                /* カセットセットＯＫならクランプ */
                                   intV = CstRls[0].intV;
                                   cTime= CstRls[0].cTime;
                                   DrvSens( LdCstWtRls, 2, 0, S_OFF,
                                            LdCstRdRls,2,0,S_OFF, 0,intV,cTime,0 );
                                   goto RETRY_STP2;
                                }
                            }
                            else if( rtn == (CST_NOT_CLAMP2 + 2000) )
                            {
                                LdCstRls( 1, 0 );
                                errRtn = LdErrMsg( rtn, ERR_LOADER_RETRY, 0 );
                            /* リトライ処理 */
                                if( errRtn == ERR_RETRY_MSG )
                                {
RETRY_CST1_2:
                                /* カセットが正常にセットされていますか */
                                    if( LdCstRdSet(1, 1) )
                                    {
                                        errRtn = LdErrMsg( CST_NOT_EXIST2,
                                                       ERR_LOADER_RETRY, 0 );
                                        if( errRtn == ERR_RETRY_MSG )
                                        {
                                            goto RETRY_CST1_2 ;
                                        }
                                    }

                            /* カセットセットＯＫならクランプ */
                                    intV = CstRls[1].intV;
                                    cTime= CstRls[1].cTime;
                                    DrvSens( LdCstWtRls, 2, 1, S_OFF,
                                            LdCstRdRls,2,1,S_OFF, 0,intV,cTime,0 );
                                    goto RETRY_STP2 ;
                                }
                            }
                            else
                            {
                                LdErrMsg( rtn, ERR_SYSTEM_RESET, 0 );
                                return( rtn );
                            }
                        }

                        if( chkIdxGoHmExceptErr(errSave) )
                        {
                        /* LdIdxGoHm で出力しないエラーのみを出力 */
                        /* for clamp */
                            LdSetClampDisAble( 0 );
                            errRtn = LdErrMsg( errSave, ERR_LOADER_RETRY,0 );
                            if( errRtn == ERR_RETRY_MSG )
                            {
                                goto RETRY1_0;
                            }
                        }
                        else
                        {
                            taskDelay( TmGetAuxTickMs(600) );
                            goto RETRY1_0;
                        }
                    }
                    else
                    {
                        if( (speedSel == 0) && chkMapErr(errSave) )
                        {
                            speedSel++;
                            goto RETRY1_1;
                        }
                        errRtn = LdErrMsg( errSave, ERR_SYSTEM_RESET,0 );
                    }
                }
            }

/* クランプーＳＷ使用禁止 */
            LdSetClampDisAble( 1 );
/*  ロットエンド後、カセットが置き直された、新しいカセットになった */
/*  mapEnd を見る必要があるか ？                                   */
            LdClrEWafDtCst( cNo );
            LdClrMapDt( cNo, mapSlot );
            for ( sN = 0; sN < mapSlot; sN++ )
            {
                if( LdGetMapStatus(cNo, sN) == WAF_EXIST )
                {
                /* ウェーハが存在する各スロットに、ポインターをセットする */
                    wPtr = LdCalCstWafPtr( cNo, sN );
                    LdSetCstWaf( cNo, sN, wPtr );
                    LdSetCstWafInit( cNo, sN, wPtr );
                /* ウェーハ個別データにセット */
                    cstPos = LdGetMoveId( cNo, sN );
                    LdSetEWafNPos( wPtr, cstPos );
                }
            }
            MapDt[cNo].mapEnd = 1;

            LdClrEWafDtWTbl( );
            LdClrEWafDtBTbl( );
            LdClrWTblDt( );

            if( LdWTblRdWaf(S_ON) == OK )
            {
                wPtr = LdCalWTblWafPtr( );
                LdSetWTblWaf( wPtr );
                LdSetEWafNPos( wPtr, W_TBL );
            }
            else
            {
                LdSetWTblWaf( WAF_EMPTY );
            }

            if( LdBTblRdWaf(S_ON) == OK )
            {
                wPtr = LdCalBTblWafPtr( );
                LdSetBTblWaf( wPtr );
                LdSetEWafNPos( wPtr, B_TBL );
            }
            else
            {
                LdSetBTblWaf( WAF_EMPTY );
            }
        }                                                   /* @12426rame-29489 */
    }

    if( MapDt[cNo].slot[sramPtr].nowExist == WAF_EMPTY )
    {
        toId = LdGetMoveId( cNo, sramPtr );
    }
    else
    {
        toId = 0;
    }
    return( toId );
}

/* @08701mtog-22229 */
/*#==================================================================#*/
/*#        Function    : LdInitPcChgUnit                             #*/
/*#        Summary     : Initial switching unit.                     #*/
/*#        Argument    : none                                        #*/
/*#        Return      : OK or error code                            #*/
/*#        Extern      : none                                        #*/
/*#        Caution     : Function only for                           #*/
/*#                               [OptionInfoGet(PO_BERNOULLI_ARM)]. #*/
/*#==================================================================#*/
intl LdInitPcChgUnit( intl pos )
{
#if( P_OCTO )                                    /* @09c11tish-25373 */
    intl rtn = OK;

    rtn = OLdInitPcChgUnit( pos );               /* @09c11tish-25373 */
    return( rtn );                               /* @09c11tish-25373 */

#else                                            /* @09c11tish-25373 */

    intl rtn, errRtn;
    intl whereIsPc;
    intl unitAct;

    unitAct = UNIT_ESC_ESC;

    while( 1 )
    {
        switch( unitAct )
        {
            case UNIT_ESC_ESC:
                rtn = DrvSens( LdUnitWtEsc,  1, WT_SWUNIT_ESC_ON,
                               LdPcEscRdPos, 1, RD_SWUNIT_ESC_ON,
                               SWCH_UNIT_ESC_DWAIT, SWCH_UNIT_ESC_INTV, SWCH_UNIT_ESC_DWAIT_CHK, 0 );
                if( rtn == OK )
                {
                    unitAct = UNIT_ESC_HM;
                    continue;
                }
                else
                {
                    rtn = ESC_MECHA_NOT_ESC_POS;
                }
                break;

            case UNIT_ESC_HM:
                rtn = DrvSens( LdUnitWtEsc,  1, WT_SWUNIT_ESC_OFF,
                               LdPcEscRdPos, 1, RD_SWUNIT_ESC_OFF,
                               SWCH_UNIT_ESC_DWAIT, SWCH_UNIT_ESC_INTV, SWCH_UNIT_ESC_DWAIT_CHK, 0 );
                if( rtn == OK )
                {
                    unitAct = UNIT_CLP;
                    continue;
                }
                else
                {
                    rtn = ESC_MECHA_NOT_HOME_POS;
                }
                break;

            case UNIT_CLP:
                rtn = DrvSens( LdUnitWtClp,  1, POS_6INCH,
                               LdPcClpRdPos, 1, RD_SWUNIT_CLMP_ON,
                               SWCH_UNIT_CLMP_DWAIT, SWCH_UNIT_CLMP_INTV, SWCH_UNIT_CLMP_DWAIT_CHK, 0 );
                if( rtn == OK )
                {
                    unitAct = UNIT_UNCLP;
                    continue;
                }
                else
                {
                    rtn = PC_SWITCH_UNIT_NOT_6POS;
                }
                break;

            case UNIT_UNCLP:
                rtn = DrvSens( LdUnitWtClp,  1, POS_8INCH,
                               LdPcClpRdPos, 1, RD_SWUNIT_CLMP_OFF,
                               SWCH_UNIT_CLMP_DWAIT, SWCH_UNIT_CLMP_INTV, SWCH_UNIT_CLMP_DWAIT_CHK, 0 );
                if( rtn == OK )
                {
                    unitAct = UNIT_DN;
                    continue;
                }
                else
                {
                    rtn = PC_SWITCH_UNIT_NOT_8POS;
                }
                break;


            case UNIT_DN:
                rtn = DrvSens( LdUnitWtZ,        1, WT_SWUNIT_DWN_ON,
                               LdPcChgUnitRdPos, 1, RD_SWUNIT_DWN_ON,
                               SWCH_UNIT_UPDN_DWAIT, SWCH_UNIT_UPDN_INTV, SWCH_UNIT_UPDN_DWAIT_CHK, 0 );
                if( rtn == OK )
                {
                    unitAct = UNIT_UP;
                    continue;
                }
                else
                {
                    rtn = PC_SWITCH_UNIT_NOT_DN_POS;
                }
                break;


            case UNIT_UP:
                rtn = DrvSens( LdUnitWtZ,        1, WT_SWUNIT_UP_ON,
                               LdPcChgUnitRdPos, 1, RD_SWUNIT_UP_ON,
                               SWCH_UNIT_UPDN_DWAIT, SWCH_UNIT_UPDN_INTV, SWCH_UNIT_UPDN_DWAIT_CHK, 0 );
                if( rtn == OK )
                {
                    unitAct = OK;
                }
                else
                {
                    rtn = PC_SWITCH_UNIT_NOT_UP_POS;
                }
                break;
        } /* switch end */

        if( rtn == OK )
        {
            if( unitAct == OK )
            {
                break;
            }
        }
        else
        {
            errRtn = LdErrMsg( rtn, ERR_LOADER_RETRY, 0 );
            if( errRtn == ERR_RETRY_MSG )
            {
                continue;
            }
            if( errRtn == LD_DIAG_ERR )
            {
                break;
            }
        }
    }

    if( rtn == OK )
    {
        whereIsPc = LdGetPcPos();
        if( whereIsPc != pos )
        {
            if( whereIsPc == ERROR )
            {
                rtn = LdChgPcPos( POS_6INCH );
            }

            if( rtn == OK )
            {
                rtn = LdChgPcPos( pos );
            }
        }
    }

    return( LdSetErr(rtn, 0) );
#endif    /* P_OCTO */                           /* @09c11tish-25373 */
}

#if( P_OCTO )                                    /* @09c11tish-25373 */
/* @09c11tish-25373 */
/*#==================================================================#*/
/*#        Function    : OLdInitPcChgUnit                            #*/
/*#        Summary     : Initial switching unit.                     #*/
/*#        Argument    : none                                        #*/
/*#        Return      : OK or error code                            #*/
/*#        Extern      : none                                        #*/
/*#        Caution     : Function only for                           #*/
/*#                               [OptionInfoGet(PO_BERNOULLI_ARM)]. #*/
/*#==================================================================#*/
intl OLdInitPcChgUnit( intl pos )
{
    intl rtn = OK;            /* return value */
    intl errRtn;              /* error return value */
    intl whereIsPc;           /* pinsette pos */
    intl unitAct;             /* unit operation */
    enum SwitchUnit           /* operation no */
    {
        MV_PC_ESC_POS,         /* 0 */
        MV_UNIT_8INCH_POS,     /* 1 */
        MV_UNIT_6INCH_POS,     /* 2 */
        MV_PC_HM_POS,          /* 3 */
    };

    unitAct = MV_PC_ESC_POS;

    while( 1 )
    {
        switch( unitAct )
        {
            case MV_PC_ESC_POS:
                /* Move pincette for escape pos. */
                rtn = LdBothPcGoEsc();
                if( rtn == OK )
                {
                    unitAct = MV_UNIT_8INCH_POS;
                    continue;
                }
                break;

            case MV_UNIT_8INCH_POS:
                /* Clamp Unit Move 8" pos and check */
                rtn = LdSwchUnitClamp( POS_8INCH );
                if( rtn == OK ) 
                {
                    unitAct = MV_UNIT_6INCH_POS;
                    continue;
                }
                break;

            case MV_UNIT_6INCH_POS:
                /* Clamp Unit Move 6" pos and check */
                rtn = LdSwchUnitClamp( POS_6INCH );
                if( rtn == OK ) 
                {
                    unitAct = MV_PC_HM_POS;
                    continue;
                }
                break;

            case MV_PC_HM_POS:
                /* Move pincette for home pos */
                rtn = LdBothPcGoHm();
                if( rtn == OK )
                {
                    unitAct = OK;
                }
                break;

        }    /* switch end */

        if( rtn == OK )
        {
            if( unitAct == OK )
            {
                /* unit initial end */
                break;
            }
        }
        else
        {
            errRtn = LdErrMsg( rtn, ERR_LOADER_RETRY, 0 );
            if( errRtn == ERR_RETRY_MSG )
            {
                continue;
            }
            if( errRtn == LD_DIAG_ERR )
            {
                break;
            }
        }
    }    /* while end */

    if( rtn == OK )
    {
        /* pinsette change */
        whereIsPc = LdGetPcPos();
        if( whereIsPc != pos )
        {
            if( whereIsPc == ERROR )
            {
                rtn = LdChgPcPos( POS_6INCH );
            }
            else if( rtn == OK )
            {
                rtn = LdChgPcPos( pos );
            }
        }
    }
    return( LdSetErr(rtn, 0) );
}

/* @11530rame-27486 */
/*#==================================================================#*/
/*#        Function    : OLdInitLaserPalnUnit                        #*/
/*#        Summary     : Initial Laser Prealign Unit.                #*/
/*#        Argument    : none                                        #*/
/*#        Return      : OK or error code                            #*/
/*#        Extern      : none                                        #*/
/*#        Caution     : Precio Octo Only.                           #*/
/*#==================================================================#*/
intl OLdInitLaserPalnUnit( void )
{
    intl rtn = OK;
    intl chk8Pos;
    intl chk6Pos;
    intl chk5Pos = 0;                                                        /* @13a30ykor-30788 : D-F7 */
    intl chk4Pos = 0;                                                        /* @13a30ykor-30788 : D-F7 */
#if(PRB3INCH)                                                                /* @14901rame-33022 */
    intl chk3Pos = 0;                                                        /* @14901rame-33022 */
#endif                                                                       /* @14901rame-33022 */
    intl wafSize;
    intl count = 0;

    if( OLdGet3PtUnitOptFlag() != FALSE )                                    /* @14c10sooy-32785 */
    {                                                                        /* @14c10sooy-32785 */
        rtn = OLdInitLaserPaln3PtUnit();                                     /* @14c10sooy-32785 */
        return( rtn );                                                       /* @14c10sooy-32785 */
    }                                                                        /* @14c10sooy-32785 */

    while( 1 )
    {
        if( LaserPalnKind == LASER_PALN_KIND_45 )                            /* @13a30ykor-30788 : D-F7 */
        {                                                                    /* @13a30ykor-30788 : D-F7 */
            chk5Pos = get_S_PalnSens5Pos();                                  /* @13a30ykor-30788 : D-F7 */
            chk4Pos = get_S_PalnSens4Pos();                                  /* @13a30ykor-30788 : D-F7 */

            if(( chk5Pos == S_ON)                                            /* @13a30ykor-30788 : D-F7 */
            && ( chk4Pos == S_OFF ))        /* Prealign Unit 5Inch Pos. */   /* @13a30ykor-30788 : D-F7 */
            {                                                                /* @13a30ykor-30788 : D-F7 */
                wafSize = 4;                                                 /* @13a30ykor-30788 : D-F7 */
            }                                                                /* @13a30ykor-30788 : D-F7 */
            else if(( chk4Pos == S_ON )                                      /* @13a30ykor-30788 : D-F7 */
                 && ( chk5Pos == S_OFF ))  /* Prealign Unit 4Inch Pos. */    /* @13a30ykor-30788 : D-F7 */
            {                                                                /* @13a30ykor-30788 : D-F7 */
                wafSize = 5;                                                 /* @13a30ykor-30788 : D-F7 */ 
            }                                                                /* @13a30ykor-30788 : D-F7 */
            else          /* The position of Prealign Unit is uncertain. */  /* @13a30ykor-30788 : D-F7 */
            {                                                                /* @13a30ykor-30788 : D-F7 */
                wafSize = 5;                                                 /* @13a30ykor-30788 : D-F7 */
                count ++;                                                    /* @13a30ykor-30788 : D-F7 */
            }                                                                /* @13a30ykor-30788 : D-F7 */
        }                                                                    /* @13a30ykor-30788 : D-F7 */
#if(PRB3INCH)                                                                /* @14901rame-33022 */
        else if( LaserPalnKind == LASER_PALN_KIND_36 )                       /* @14901rame-33022 */
        {                                                                    /* @14901rame-33022 */
            chk6Pos = get_S_PalnSens6Pos();                                  /* @14901rame-33022 */
            chk3Pos = get_S_PalnSens3Pos();                                  /* @14901rame-33022 */

            if(( chk6Pos == S_ON)                                            /* @14901rame-33022 */
            && ( chk3Pos == S_OFF ))        /* Prealign Unit 6Inch Pos. */   /* @14901rame-33022 */
            {                                                                /* @14901rame-33022 */
                wafSize = 3;                                                 /* @14901rame-33022 */
            }                                                                /* @14901rame-33022 */
            else if(( chk3Pos == S_ON )                                      /* @14901rame-33022 */
                 && ( chk6Pos == S_OFF ))  /* Prealign Unit 3Inch Pos. */    /* @14901rame-33022 */
            {                                                                /* @14901rame-33022 */
                wafSize = 6;                                                 /* @14901rame-33022 */
            }                                                                /* @14901rame-33022 */
            else          /* The position of Prealign Unit is uncertain. */  /* @14901rame-33022 */
            {                                                                /* @14901rame-33022 */
                wafSize = 6;                                                 /* @14901rame-33022 */
                count ++;                                                    /* @14901rame-33022 */
            }                                                                /* @14901rame-33022 */
        }                                                                    /* @14901rame-33022 */
#endif                                                                       /* @14901rame-33022 */
        else if( LaserPalnKind == LASER_PALN_KIND_46 )                       /* @20115nsuz-41929 */
        {                                                                    /* @20115nsuz-41929 */
            chk6Pos = get_S_PalnSens6Pos();                                  /* @20115nsuz-41929 */
            chk4Pos = get_S_PalnSens4Pos();                                  /* @20115nsuz-41929 */

            if(( chk6Pos == S_ON)                                            /* @20115nsuz-41929 */
            && ( chk4Pos == S_OFF ))        /* Prealign Unit 6Inch Pos. */   /* @20115nsuz-41929 */
            {                                                                /* @20115nsuz-41929 */
                wafSize = 4;                                                 /* @20115nsuz-41929 */
            }                                                                /* @20115nsuz-41929 */
            else if(( chk4Pos == S_ON )                                      /* @20115nsuz-41929 */
                 && ( chk6Pos == S_OFF ))   /* Prealign Unit 4Inch Pos. */   /* @20115nsuz-41929 */
            {                                                                /* @20115nsuz-41929 */
                wafSize = 6;                                                 /* @20115nsuz-41929 */
            }                                                                /* @20115nsuz-41929 */
            else          /* The position of Prealign Unit is uncertain. */  /* @20115nsuz-41929 */
            {                                                                /* @20115nsuz-41929 */
                wafSize = 6;                                                 /* @20115nsuz-41929 */
                count ++;                                                    /* @20115nsuz-41929 */
            }                                                                /* @20115nsuz-41929 */
        }                                                                    /* @20115nsuz-41929 */
        else                                                                 /* @13a30ykor-30788 : D-F7 */
        {                                                                    /* @13a30ykor-30788 : D-F7 */
            chk8Pos = get_S_PalnSens8Pos();
            chk6Pos = get_S_PalnSens6Pos();

            if(( chk8Pos == S_ON) && ( chk6Pos == S_OFF ))        /* Prealign Unit 8Inch Pos. */
            {
                wafSize = 6;
            }
            else if(( chk6Pos == S_ON ) && ( chk8Pos ==  S_OFF )) /* Prealign Unit 6Inch Pos. */
            {
                wafSize = 8;
            }
            else           /* The position of Prealign Unit is uncertain.  */
            {
                wafSize = 8;
                count ++;
            }
        }                                                                    /* @13a30ykor-30788 : D-F7 */    

        rtn = OLdPalnSld( S_ON, wafSize );

        if(( rtn ) || ( count >= 1 ))
        {
            break;
        }
        count ++;
    }

    OLdLaserWtPaln( LASER_LED_OFF, wafSize );

    return( rtn );
}
#endif    /* P_OCTO */                           /* @09c11tish-25373 */

/* @12808ykor-29900 : P-F3 */
/*#==================================================================#*/
/*#        Function    : LdChkWafOnPc                                #*/
/*#        Summary     : Pincette上のWaferをVaccumでCheckする        #*/
/*#        Argument    : intl  usePc; 使用Pincette                   #*/
/*#        Return      : OK or ERROR                                 #*/
/*#        Extern      :                                             #*/
/*#        Caution     :                                             #*/
/*#==================================================================#*/
intl LdChkWafOnPc( intl usePc )
{
    intl    dWait=0;
    intl    intV=0;
    intl    cTime=0;
    intl    rtn = OK;

    dWait  = PcVac [ usePc ].dWait;
    intV  = PcVac [ usePc ].intV;
    cTime = PcVac [ usePc ].cTime;

    rtn = DrvSens( LdPcWtVac,2,usePc,S_ON,
                   LdPcRdVac,2,usePc,S_ON, dWait, intV, cTime , 0);
    if( rtn != OK )
    {
        LdPcWtVac( usePc, S_OFF );
    }

    return( rtn );
}

/* @12808ykor-29900 : P-F4 */
/*#==================================================================#*/
/*#        Function    : LdSc2PcWafPtrAndSram                        #*/
/*#        Summary     : SubchuckからPincetteにウェーハの移動履歴を  #*/
/*#                      移動する                                    #*/
/*#        Argument    : intl  usePc; 使用Pincette                   #*/
/*#                    : intl  *pcWafOnFlg; PC上のWafer有り判定フラグ#*/
/*#        Return      : OK or ERROR                                 #*/
/*#        Extern      :                                             #*/
/*#        Caution     :                                             #*/
/*#==================================================================#*/
intl LdSc2PcWafPtrAndSram( intl usePc, intl *pcWafOnFlg )
{
    intl rtn = OK;
    intl ptrSc = -1;
    intl rtnVal = OK;
    intl sRamPcPtr = 0;
    intl sRamScPtr = 0;
    intl sRamNo = 0;
    intl wafOnBit = 0x00;

    rtn = LdChkWafOnPc( usePc );
    if( rtn == OK )
    {
        if( usePc == UPPER )
        {
            sRamPcPtr = SramGet4( UpcWaf );
            sRamNo = UpcWaf;
            wafOnBit = 0x01;
        }
        else
        {
            sRamPcPtr = SramGet4( LpcWaf );
            sRamNo = LpcWaf;
            wafOnBit = 0x02;
        }

        sRamScPtr = SramGet4( SckWaf );

        if( ( sRamPcPtr == 0x00 )              /* PCのウェーハ移動履歴（Sram）が空 */
         || ( sRamPcPtr == sRamScPtr ) )       /* PCとSCのウェーハ移動履歴（Sram）が同じ */
        {
            rtnVal = LdChkScWaf( WAF_EXIST, &ptrSc );
            if( rtnVal == OK )
            {
                LdSetPcWaf( usePc, ptrSc );
                LdSetScWaf( WAF_EMPTY );
            }

            if( ( sRamScPtr != 0x00 )
             && ( sRamScPtr != 0xFF ) )
            {
                SramSet4( sRamNo, sRamScPtr );
                SramSet4( SckWaf, 0 );
            }
            *pcWafOnFlg |= wafOnBit;
        }
    }

    return( rtn );
}

#if( P_OCTO )                                    /* @12b16rame-29941 */
/* @12b16rame-29941 */
/*#==================================================================#*/
/*#        Function    : LdIdxNozzleHome_Initial                     #*/
/*#        Summary     : イニシャル時のインデクサエアアシストノズルの#*/
/*#                    : 位置を初期化(ホーム位置駆動&位置確認)します #*/
/*#        Argument    : cstNo                                       #*/
/*#        Return      : OK or Error code                            #*/
/*#        Extern      :                                             #*/
/*#        Caution     :                                             #*/
/*#==================================================================#*/
intl    LdIdxNozzleHome_Initial( intl cstNo )
{
    intl    nzlHome = 0;
    intl    nzlSet = 0;
    intl    rtn = OK;

    if( WarpedLoader == TRUE )
    {
        /* Nozzle HOME Pos drive. */
        rtn = LdIdxNozzlePos( cstNo, NZL_HOME, 1 );

        if( rtn != OK )
        {
            return( rtn );
        }

        /* Nozzle HOME & SET Pos check. */
        nzlHome =  LdIdxNozzleRdSol( cstNo, NZL_HOME );
        nzlSet = LdIdxNozzleRdSol( cstNo, NZL_SET );

        if(!(( nzlHome == OK ) && ( nzlSet == ERROR )))
        {
            /* Nozzleのhome位置sensorがON、 */
            /* set位置sensorがOFF以外の状態 */
            rtn = IDX_NOZZLE_NOT_HOME;
        }
    }
    return( LdSetErr( rtn, 0 ) );
}
#endif                                           /* @12b16rame-29941 */

#if( P_OCTO )                                           /* @14c10sooy-32785 */
/* @14c10sooy-32785 */
/*#========================================================================#*/
/*#        Function    : OLdInitLaserPalnTSUnit                            #*/
/*#        Summary     : レーザープリアライン3段階ユニットのイニシャル動作 #*/
/*#        Argument    : void                                              #*/
/*#        Return      : intl 0:OK / !0:Error Code                         #*/
/*#        Extern      :                                                   #*/
/*#        Caution     :                                                   #*/
/*#========================================================================#*/
intl    OLdInitLaserPaln3PtUnit( void )
{
    intl rtn = OK;
    intl wafMaxSize = 0;
    intl wafMinSize = 0;
    intl chkPos = OLdChkLaserPaln3PtUnitPos( );

    if( LaserPalnKind == LASER_PALN_KIND_458 )
    {
        wafMaxSize = 8;
        wafMinSize = 4;
    }
    else if( LaserPalnKind == LASER_PALN_KIND_468 )             /* @16530sooy-36978 */
    {                                                           /* @16530sooy-36978 */
        wafMaxSize = 8;                                         /* @16530sooy-36978 */
        wafMinSize = 4;                                         /* @16530sooy-36978 */
    }                                                           /* @16530sooy-36978 */
    else if( LaserPalnKind == LASER_PALN_KIND_568 )             /* @22405etak-T15632 */
    {                                                           /* @22405etak-T15632 */
        wafMaxSize = 8;                                         /* @22405etak-T15632 */
        wafMinSize = 5;                                         /* @22405etak-T15632 */
    }                                                           /* @22405etak-T15632 */

    if( chkPos == wafMaxSize )
    {
        rtn = OLdPalnSld( S_ON, wafMinSize );

        if( rtn )
        {
            goto EXIT;
        }
    }

    rtn = OLdPalnSld( S_ON, wafMaxSize );
    OLdWtLaser3PtUnitLock( S_OFF );

EXIT:
    return( rtn );
}
#endif                                                  /* @14c10sooy-32785 */

#endif 							      /* @06c13mtog-17805 */

#if( P_OCTO )                                       /* @21524thos-42311 */
/* @21524thos-42311 */
/*#--------------------------------------------------------------------#*/
/*#        Function    : LdAllDriver_Init()                            #*/
/*#        Summary     : ローダドライバ復旧                            #*/
/*#                    : (ローダ各軸ドライバ初期化&各軸原点位置駆動&   #*/
/*#                    :  各軸ホーム位置駆動)                          #*/
/*#        Argument    : none                                          #*/
/*#        Return      : OK                                            #*/
/*#        Extern      :                                               #*/
/*#        Caution     : 本関数はSimpleInit()を参考に作成しております。#*/
/*#                      その為、SimpleInit()に変更を加える際、        #*/
/*#                      SimpleInit()の変更が本関数にも必要となる      #*/
/*#                      可能性があります。SimpleInit()に変更を        #*/
/*#                      加える際は、本関数の確認もお願いします。      #*/
/*#--------------------------------------------------------------------#*/
intl LdAllDriver_Init( void )
{
    intl rtn = 0, errRtn = 0;
    intl intV;
    intl cTime;
    intl scExistErr = 0;
    intl scwaf = WAF_EMPTY;

    /* Boot Moter Driver */
    OLdCEMarkSioMotorBoot();

    /* Delay */
    taskDelay( TmGetAuxTickMs( LdRecoverMtrBootDelay ) );

    /* Sub-Chuck Z Recover */
    if(( rtn = LdScZDriver_Init( &scExistErr, &scwaf ) ))
    {
        return ( rtn );
    }

E_RETRY1:
    /* アッパピンセット原点位置駆動 */
    if( ( rtn = LdPcGoOrg( UPPER ) ) )
    {
        errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 );
        if ( errRtn == ERR_RETRY_MSG )
        {
            goto E_RETRY1;
        }
        if ( errRtn == LD_DIAG_ERR )
        {
            return ( rtn );
        }
    }

    /* アッパピンセットホーム位置駆動 */
    if( ( rtn = LdPcGoHm( UPPER ) ) )
    {
        errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 );
        if ( errRtn == ERR_RETRY_MSG )
        {
            goto E_RETRY1;
        }
        if ( errRtn == LD_DIAG_ERR )
        {
            return ( rtn );
        }
    }

E_RETRY2:
    /* ロアピンセット原点位置駆動 */
    if( ( rtn = LdPcGoOrg( LOWER ) ) )
    {
        errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 );
        if ( errRtn == ERR_RETRY_MSG )
        {
            goto E_RETRY2;
        }
        if ( errRtn == LD_DIAG_ERR )
        {
            return ( rtn );
        }
    }
    /* ロアピンセットホーム位置駆動 */
    if( ( rtn = LdPcGoHm( LOWER ) ) )
    {
        errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 );
        if ( errRtn == ERR_RETRY_MSG )
        {
            goto E_RETRY2;
        }
        if ( errRtn == LD_DIAG_ERR )
        {
            return ( rtn );
        }
    }

    if( OptionInfoGet(PO_BERNOULLI_ARM) == 1 )
    {
        /* イニシャル時サブチャック上のウェーハ有無確認関数 */
        rtn = LdReturnScWaf_Brnl( scExistErr, scwaf );
        if( rtn != OK )
        {
            return( LdSetErr(rtn, 0) );
        }
    }


E_RETRY3:
    /* ローダθ原点位置駆動 */
    if( ( rtn = LdPqGoOrg() ) )
    {
        errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 );
        if ( errRtn == ERR_RETRY_MSG )
        {
            goto E_RETRY3;
        }
        if ( errRtn == LD_DIAG_ERR )
        {
            return ( rtn );
        }
    }
    /* ローダθホーム位置駆動 */
    if( ( rtn = LdPqGoHm() ) )
    {
        errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 );
        if ( errRtn == ERR_RETRY_MSG )
        {
            goto E_RETRY3;
        }
        if ( errRtn == LD_DIAG_ERR )
        {
            return ( rtn );
        }
    }

    /* インデクサが動作可能かピンセット位置をチェック */
    if( ( rtn = LdChkPcIdxMove() ) )
    {
         return( LdSetErr( rtn, 0 ) );
    }

    /* インデクサを動かすので、チルトします */
    if( OptionInfoGet( PO_2ND1CST ) )
    {
        /* カセット2をチルト */
        if( ( rtn = LdCstTilt( 1, 0 ) ) )
        {
            return( rtn );
        }
    }
    else
    {
        /* カセット1をチルト */
        if( ( rtn = LdCstTilt( 0, 0 ) ) )
        {
            return( rtn );
        }
    }

    if( OptionInfoGet( PO_2NDLOADER ) )
    {
        /* カセット2をチルト */
        if ( (rtn = LdCstTilt ( 1, 0 )) )
        {
            return ( rtn );
        }
    }

E_RETRY4:
    /* インデクサ停止仕様 */
    if( (LdCstRdSet(0, 1) == OK) || (LdCstRdSet(1, 1) == OK) )
    {
        MQSend( LdIdxStopTask, "CHK_CLP", WAIT_FOREVER, 0 ) ;
    }
    rtn = LdIdxGoOrg();
    /* インデクサ停止仕様 */
    MQSend( LdIdxStopTask, "END_CLP", WAIT_FOREVER, 0 ) ;
    if ( rtn )
    {
        if( rtn == (CST_NOT_CLAMP1 + 2000) )
        {
            LdCstRls( 0, 0 ) ;
        }
        else if( rtn == (CST_NOT_CLAMP2 + 2000) )
        {
            LdCstRls( 1, 0 ) ;
        }
        else    return( rtn ) ;

        errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 );
        if ( errRtn == ERR_RETRY_MSG )
        {
            if( rtn == (CST_NOT_CLAMP1 + 2000) )
            {
                intV = CstRls [ 0 ].intV;
                cTime= CstRls [ 0 ].cTime;

                DrvSens( LdCstWtRls, 2, 0, S_OFF,
                       LdCstRdRls,2, 0, S_OFF, 0 , intV, cTime , 0 );
            }
            else if( rtn == (CST_NOT_CLAMP2 + 2000) )
            {
                intV = CstRls [ 1 ].intV;
                cTime= CstRls [ 1 ].cTime;

                DrvSens( LdCstWtRls, 2, 1, S_OFF,
                       LdCstRdRls,2, 1, S_OFF, 0 , intV, cTime , 0 );
            }
            goto E_RETRY4;
        }
        if ( errRtn == LD_DIAG_ERR )
        {
             return ( rtn );
        }
    }

    /* インデクサ停止仕様 */
    if( (LdCstRdSet(0, 1) == OK) || (LdCstRdSet(1, 1) == OK) )
    {
        MQSend( LdIdxStopTask, "CHK_CLP", WAIT_FOREVER, 0 ) ;
    }
    rtn = LdIdxGoHm();
    /* インデクサ停止仕様 */
    MQSend( LdIdxStopTask, "END_CLP", WAIT_FOREVER, 0 ) ;
    if ( rtn )
    {
        if( rtn == (CST_NOT_CLAMP1 + 2000) )
        {
            LdCstRls( 0, 0 ) ;
        }
        else if( rtn == (CST_NOT_CLAMP2 + 2000) )
        {
            LdCstRls( 1, 0 ) ;
        }
        else    return( rtn ) ;

        errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 ) ;
        if ( errRtn == ERR_RETRY_MSG )
        {
            if( rtn == (CST_NOT_CLAMP1 + 2000) )
            {
                intV = CstRls [ 0 ].intV;
                cTime= CstRls [ 0 ].cTime;

                DrvSens( LdCstWtRls, 2, 0, S_OFF,
                      LdCstRdRls,2, 0, S_OFF, 0 , intV, cTime , 0 );
            }
            else if( rtn == (CST_NOT_CLAMP2 + 2000) )
            {
                intV = CstRls [ 1 ].intV;
                cTime= CstRls [ 1 ].cTime;

                DrvSens( LdCstWtRls, 2, 1, S_OFF,
                      LdCstRdRls,2, 1, S_OFF, 0 , intV, cTime , 0 );
            }
            goto E_RETRY4;
        }
        else if ( errRtn == LD_DIAG_ERR ) return ( rtn );
    }

    if(OptionInfoGet(PO_2ND1CST))
    {
        /* カセット2をフラット */
        if((rtn = LdCstFlat(1,0)))
        {
            return(rtn);
        }
    }
    else
    {
        /* カセット1をフラット */
        if((rtn = LdCstFlat(0,0)))
        {
            return(rtn);
        }
    }
    if(OptionInfoGet(PO_2NDLOADER))
    {
        /* カセット2をフラット */
        if((rtn = LdCstFlat(1,0)))
        {
            return(rtn);
        }
    }

    return( OK );
}

/* @21524thos-42311 */
/*#========================================================================#*/
/*# Function    : LdDriverRecoverSub                                       #*/
/*# Summary     :                                                          #*/
/*# Argument    : void                                                     #*/
/*# Return      : OK or ERROR                                              #*/
/*# Caution     :                                                          #*/
/*#========================================================================#*/
intl LdDriverRecoverSub( void )
{
    intl    rtn, errRtn;
    intl    pc = -1, step = 0;
    intl    pqPos = 0, pqStep = 0;
    intl    idxPos = 0, idxStep = 0;
    intl    scZPos = 0, scWaf = 0;
    intl    uPcPos = 0, uPcStep = 0;
    intl    lPcStep = 0;
    intl    intV, cTime;

    if( LD_RECOVER_DEBUG )
    {
        logMsg("LdDriverRecoverSub Start.\n" );
    }

    LD_STG_SAFE = 0;

    /* 復旧後のローダを元の位置に戻す場合、 */
    /* 復旧前に元のローダ位置を記録 */
    /* Pincette θ Position Save */
    pqPos = LdPartDt.pqPos;
    pqStep = LdPartDt.pqStep;

    /* Indexer Position Save */
    idxPos = LdPartDt.idxPos;
    idxStep = LdPartDt.idxStep;

    /* Sub-Chuck Z Position Save */
    scWaf = LdPartDt.scWaf;
    scZPos = LdPartDt.scZPos;

    /* Pincette Step Save */
    uPcStep = LdPartDt.uPcStep;
    lPcStep = LdPartDt.lPcStep;

    if( Sys_mode & SYS_MANID )
    {
        /* Upper Pincette Position Save */
        uPcPos = LdPartDt.uPcPos;
    }

    if( LD_RECOVER_DEBUG )
    {
        logMsg("before Loader Recovery:\n");
        LdRecoverPartDtDisp(0);
        logMsg("Save Position:\n");
        logMsg("pqPos = %d\n", pqPos );
        logMsg("pqStep = %d\n", pqStep );
        logMsg("idxPos = %d\n", idxPos );
        logMsg("idxStep = %d\n", idxStep );
        logMsg("scWaf = %d\n", scWaf );
        logMsg("scZPos = %d\n", scZPos );
        logMsg("uPcPos = %d\n", uPcPos );
        logMsg("uPcStep = %d\n", uPcStep );
        logMsg("lPcStep = %d\n", lPcStep );
    }

    /* 復旧処理 */
    rtn = LdAllDriver_Init();

    /* Position Disp */
    if( LD_RECOVER_DEBUG )
    {
        logMsg("After Loader Recovery:\n");
        LdRecoverPartDtDisp(0);
    }

    /* 復旧処理が正常終了後、各軸を元の位置に戻す */
    if( rtn == OK )
    {
        if( LD_RECOVER_DEBUG )
        {
            logMsg("Move to Previous Position:\n");
        }

        /* 復旧後のローダを元の位置に戻す場合、 */
        /* ローダを復旧前の位置に戻す */
        /* Pincette θ Move */
        if( pqPos != 0 )
        {
RETRY_1:
            rtn = LdPqGoAddr( pqStep );
            if( rtn != OK )
            {
                errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 );
                if( errRtn == ERR_RETRY_MSG )
                {
E_RETRY_PQ_ORG:         /* ローダθの原点駆動処理 */
                    if( ( rtn = LdPqGoOrg() ) )
                    {
                        errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 );
                        if ( errRtn == ERR_RETRY_MSG )
                        {
                            goto E_RETRY_PQ_ORG;
                        }
                        if ( errRtn == LD_DIAG_ERR )
                        {
                            return ( rtn );
                        }
                    }
                    goto RETRY_1;
                }
                if ( errRtn == LD_DIAG_ERR )
                {
                    return ( rtn );
                }
            }
            LdPartDt.pqPos = pqPos;

            /* Pincette θ Position Disp */
            LdRecoverPartDtDisp(MTR_PQ);
        }

        /* Indexer Move */
        if( idxPos != 0 )
        {
RETRY_2:
            /* インデクサ停止仕様 */
            if( (LdCstRdSet(0, 1) == OK) || (LdCstRdSet(1, 1) == OK) )
            {
                MQSend( LdIdxStopTask, "CHK_CLP", WAIT_FOREVER, 0 ) ;
            }
            /* インデクサーを復旧前の位置に移動 */
            rtn = LdIdxGoAddr( idxStep );
            /* インデクサ停止仕様 */
            MQSend( LdIdxStopTask, "END_CLP", WAIT_FOREVER, 0 ) ;
            if( rtn != OK )
            {
                if( rtn == (CST_NOT_CLAMP1 + 2000) )
                {
                    LdCstRls( 0, 0 ) ;
                }
                else if( rtn == (CST_NOT_CLAMP2 + 2000) )
                {
                    LdCstRls( 1, 0 ) ;
                }
                else    return( rtn ) ;

                errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 );
                if ( errRtn == ERR_RETRY_MSG )
                {
                    if( rtn == (CST_NOT_CLAMP1 + 2000) )
                    {
                        intV = CstRls [ 0 ].intV;
                        cTime= CstRls [ 0 ].cTime;

                        DrvSens( LdCstWtRls, 2, 0, S_OFF,
                               LdCstRdRls,2, 0, S_OFF, 0 , intV, cTime , 0 );
                    }
                    else if( rtn == (CST_NOT_CLAMP2 + 2000) )
                    {
                        intV = CstRls [ 1 ].intV;
                        cTime= CstRls [ 1 ].cTime;

                        DrvSens( LdCstWtRls, 2, 1, S_OFF,
                               LdCstRdRls,2, 1, S_OFF, 0 , intV, cTime , 0 );
                    }
RETRY_IDX_ORG:
                    /* インデクサ停止仕様 */
                    if( (LdCstRdSet(0, 1) == OK) || (LdCstRdSet(1, 1) == OK) )
                    {
                        MQSend( LdIdxStopTask, "CHK_CLP", WAIT_FOREVER, 0 ) ;
                    }
                    /* インデクサーの原点駆動 */
                    rtn = LdIdxGoOrg();
                    /* インデクサ停止仕様 */
                    MQSend( LdIdxStopTask, "END_CLP", WAIT_FOREVER, 0 ) ;
                    if ( rtn )
                    {
                        if( rtn == (CST_NOT_CLAMP1 + 2000) )
                        {
                            LdCstRls( 0, 0 ) ;
                        }
                        else if( rtn == (CST_NOT_CLAMP2 + 2000) )
                        {
                            LdCstRls( 1, 0 ) ;
                        }
                        else    return( rtn ) ;

                        errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 );
                        if ( errRtn == ERR_RETRY_MSG )
                        {
                            if( rtn == (CST_NOT_CLAMP1 + 2000) )
                            {
                                intV = CstRls [ 0 ].intV;
                                cTime= CstRls [ 0 ].cTime;

                                DrvSens( LdCstWtRls, 2, 0, S_OFF,
                                       LdCstRdRls,2, 0, S_OFF, 0 , intV, cTime , 0 );
                            }
                            else if( rtn == (CST_NOT_CLAMP2 + 2000) )
                            {
                                intV = CstRls [ 1 ].intV;
                                cTime= CstRls [ 1 ].cTime;

                                DrvSens( LdCstWtRls, 2, 1, S_OFF,
                                       LdCstRdRls,2, 1, S_OFF, 0 , intV, cTime , 0 );
                            }
                            goto RETRY_IDX_ORG;
                        }
                        if ( errRtn == LD_DIAG_ERR )
                        {
                             return ( rtn );
                        }
                    }
                    goto RETRY_2;
                }
                if ( errRtn == LD_DIAG_ERR )
                {
                    return ( rtn );
                }
            }
            /* インデクサーを復旧前の位置に戻した後、ポジションも復旧前の位置に更新 */
            LdPartDt.idxPos = idxPos;

            /* Indexer Position Disp */
            LdRecoverPartDtDisp(MTR_IDX);
        }

        /* Sub-Chuck Z Move */
        if( scWaf != -1 ) /* サブチャック上にウェーハあり */
        {
            /* アッパピンセットより高い位置 */
            if( scZPos == SCZ_UUP )
            {
                pc = UPPER;
                step = uPcStep;
            }
            /* ロアピンセットより高い位置 */
            else if( scZPos == SCZ_LUP )
            {
                pc = LOWER;
                step = lPcStep;
            }

            if( pc != -1 ) /* 初期値以外 */
            {
                rtn = LdSubPc2Sc( pc, step );
                if( rtn != OK )
                {
                    LdErrMsg( rtn, ERR_SYSTEM_INITIAL, 0 );
                    return( rtn );
                }
                LdPartDt.scZPos = scZPos;
            }

            /* Sub-Chuck Z Position Disp */
            LdRecoverPartDtDisp(MTR_SCZ);
        }

        if( Sys_mode & SYS_MANID )
        {
RETRY_3: /* ID読み取りエラー発生中の為、UPCをID読み取り位置へ戻す */
            rtn = LdPcGoAddr( UPPER, uPcStep );
            if( rtn != OK )
            {
                errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 );
                if( errRtn == ERR_RETRY_MSG )
                {
RETRY_UPC: /* アッパピンセット原点位置駆動 */
                    if( ( rtn = LdPcGoOrg( UPPER ) ) )
                    {
                        errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 );
                        if ( errRtn == ERR_RETRY_MSG )
                        {
                            goto RETRY_UPC;
                        }
                        if ( errRtn == LD_DIAG_ERR )
                        {
                            return ( rtn );
                        }
                    }
                    goto RETRY_3;
                }
                if ( errRtn == LD_DIAG_ERR )
                {
                    return ( rtn );
                }
            }
            LdPartDt.uPcPos = uPcPos;

            /* Upper Pincette Position Disp */
            LdRecoverPartDtDisp(MTR_UPC);
        }
    }

    return( rtn );
}

/* @10b04tnak-26887 *//* @21524thos-42311 */
/*#========================================================================#*/
/*# Function    : LdRecoverHInt                                            #*/
/*# Summary     : #*/
/*# Argument    : mode : 0=setup motor , 1=setup & initialize motor        #*/
/*# Return      : ERROR                                                    #*/
/*# Caution     : The cover is locked. However,the cover is not unlocked.  #*/
/*#========================================================================#*/
intl LdRecoverHInt(intl mode)
{
    intl rtn=0;
/* @21524thos-42311    intl drivePos = 99; */

    if(!OptionInfoGet(PO_NEW_CE_MARKING_SPEC))
    {
        return(0);
    }

    if(( mode == 1 ) &&                 /* モータイニシャライズ実行   *//* @21524thos-42311 */
       ( LdChkBothPcHm() == ERROR ) &&  /* ピンセットが延びている     *//* @21524thos-42311 */
       ( !(Sys_mode & SYS_MANID) ))     /* ID読み取りエラー発生時以外 *//* @21524thos-42311 */
    {                                                                   /* @21524thos-42311 */
        return(0);                                                      /* @21524thos-42311 */
    }                                                                   /* @21524thos-42311 */

    if(!LdChkStgCvCloseHIntR())    /* HP & SACC cover Close Check */
    {
        if(!LdChkCvCloseHIntR())   /* Casset cover Close Check */
        {
            if(LdChkIntStsHInt())  /* Interlock is occurred. */
            {
                LdClrCvStsHInt();  /* clear cover open status */
                if(mode)
                {
/* @21524thos-42311                    drivePos = LdReturnPos(); */
/* @21524thos-42311                    rtn = LdDriverRecoverSub(drivePos); */
                    rtn = LdDriverRecoverSub();       /* @21524thos-42311 */
                }
            }
        }
    }
    return(rtn);
}

/* @10b04tnak-26887 *//* @21524thos-42311 */
/*#========================================================================#*/
/*# Function    : LdChkCvCloseHIntR                                        #*/
/*# Summary     : #*/
/*# Argument    : #*/
/*# Return      : #*/
/*#========================================================================#*/
intl LdChkCvCloseHIntR(void)
{
    intl rtn=0,errRtn=0;

E_RETRY:
    if((rtn = LdChkCvCloseHInt()))
    {
        errRtn = LdErrMsg(rtn,ERR_LOADER_RETRY,0);
        if(errRtn == ERR_RETRY_MSG)
        {
            goto E_RETRY;
        }
        if(errRtn == LD_DIAG_ERR)
        {
            return(LdSetErr(rtn,0));
        }
    }

    return(rtn);
}
/* @13425tnak-30599 *//* @21524thos-42311 */
/*#========================================================================#*/
/*# Function    : LdChkStgCvCloseHIntR                                     #*/
/*# Summary     : #*/
/*# Argument    : #*/
/*# Return      : #*/
/*#========================================================================#*/
intl LdChkStgCvCloseHIntR(void)
{
    intl rtn=0;
    intl errRtn=0;

E_RETRY:
    if((rtn = LdChkStgCvCloseHInt()))
    {
        errRtn = LdErrMsg(rtn,ERR_LOADER_RETRY,0);
        if(errRtn == ERR_RETRY_MSG)
        {
            goto E_RETRY;
        }
        if(errRtn == LD_DIAG_ERR)
        {
            return(LdSetErr(rtn,0));
        }
    }

    return(rtn);
}

/* @21524thos-42311 */
/*#========================================================================#*/
/*# Function    : LdScZDriver_Init                                         #*/
/*# Summary     : サブチャックZの復旧処理                                  #*/
/*# Argument    : void                                                     #*/
/*# Return      : OK or Error Code                                         #*/
/*#========================================================================#*/
intl LdScZDriver_Init( intl *scExistErr, intl *scwaf )
{
    intl rtn = 0, errRtn = 0;
    intl ptrSc = -1;
    intl vacErrRetry;
    intl dWait = ScVac.dWait;
    intl intV  = ScVac.intV;
    intl cNo = 0;
    intl airContSc;
    intl scSuctionType;
    intl airContBkPc;
    intl dummyPtr;
    intl sRamPtr;
    intl bernolliArm;
    intl bernoulliWafBack;
    intl subchuckZMtr;
    intl pcWafOnFlg = 0;

    bernolliArm = OptionInfoGet(PO_BERNOULLI_ARM);
    bernoulliWafBack = OptionInfoGet(PO_BERNOULLI_WAFBACK);
    subchuckZMtr = OptionInfoGet( PO_SUBCHUCKZ );
    airContSc = chkBer_ScAirCont();
    airContBkPc = chkBkSideBer_PcAirCont();
    scSuctionType = ChkSuctionType_Sc();

    /* サブチャックZがアッパピンセットもしくはロアピンセットと同じ高さである場合 */
    /* サブチャックZがアッパピンセットもしくはロアピンセットよりも高い位置にある場合 */
    if( (( LdPartDt.scZPos == SCZ_UPC ) || ( LdPartDt.scZPos == SCZ_LPC )) ||
        (( LdPartDt.scZPos == SCZ_UUP ) || ( LdPartDt.scZPos == SCZ_LUP )) )
    {
        do
        {
            vacErrRetry = 0 ;
            /* Bernoulli Subchuckのエア圧力制御機能の設定確認 が有効 */
            if( airContSc == 1 )
            {
                rtn = LdScRdVac( S_ON );
            }
            else
            {
                /* サブチャックのバキュームオン */
                rtn = DrvSens( LdScWtVac , 1 , S_ON ,
                               LdScRdVac , 1 , S_ON , dWait, intV, intV * 3 , 0 );
            }

            /* サブチャック上でウェーハを吸着中 */
            if( rtn == OK )
            {
                /* Bernoulli Subchuckのエア圧力制御機能の設定確認 が有効 */
                if( airContSc == 1 )
                {
                    /* ウェーハを吸着しているか分からない為、エアON */
                    if(( rtn = LdScAirPresCont_OnOff( S_ON, AIR_PRES_CONT,
                                               PcAirPresDt.PcAirCst[cNo].scPcAir,
                                               PcAirPresDt.PcAirCst[cNo].scPcAirTime )))
                    {
                        return( rtn );
                    }
                }

                /* サブチャック上にウェーハあり */
                if ( LdChkScWaf ( WAF_EXIST, &ptrSc ) )
                {
                    *scExistErr = SC_WAF_NOT_RETURN;
                }

                /* ハードオプション「ベルヌーイ(ウェーハ上面搬送)」:あり */
                if( bernolliArm == 1 )
                {
                    /* サブチャックの状態をウェーハありに設定 */
                    *scwaf = WAF_EXIST;
                }
                else
                {
                    /* ハードオプション「サブチャックZ」:あり */
                    if( subchuckZMtr )
                    {
                        /* アッパピンセットとロアピンセットのバキュームがONである場合 */
                        if((LdPcRdVac( UPPER, 1 ) == OK)
                            ||(LdPcRdVac( LOWER, 1) == OK))
                        {
                            /* ハードオプション「ベルヌーイ(ウェーハ下面搬送)」:なし */
                            if( bernoulliWafBack == 0 )
                            {
                                /* バキュームサブチャック実装 */
                                if( scSuctionType == VACUUM_SC )
                                {
                                    if(( rtn = LdScVacOff( 1 )))
                                    {
                                        return( rtn );
                                    }
                                }
                                else
                                {
                                    LdScWtVac( S_OFF );
                                }
                            }

                            if( ( LdChkBothPcHm() == OK ) )
                            {
                                if( bernoulliWafBack == 1 )
                                {
                                    /* ベルヌーイサブチャック実装 */
                                    if( scSuctionType == BERNOULLI_SC )
                                    {
                                        if( airContSc == 1 )
                                        {
                                            if(( rtn = LdScAirPresCont_OnOff( S_OFF, AIR_PRES_CONT,
                                                                              PcAirPresDt.PcAirCst[cNo].scPcAir,
                                                                              PcAirPresDt.PcAirCst[cNo].scPcAirTime )))
                                            {
                                                return( rtn );
                                            }
                                        }
                                        else
                                        {
                                            LdScWtVac ( S_OFF );
                                        }
                                    }
                                    else
                                    {
                                        if(( rtn = LdScVacOff( 1 )))
                                        {
                                            return( rtn );
                                        }
                                    }

                                    if( airContBkPc == 1 )
                                    {
                                        if(( rtn = LdPcAirPresCont_OnOff( UPPER, S_ON, AIR_PRES_CONT,
                                                                          PcAirPresDt.PcAirCst[cNo].pcScAir[UPPER],
                                                                          PcAirPresDt.PcAirCst[cNo].pcScAirTime[UPPER] )))
                                        {
                                            return( rtn );
                                        }
                                        if(( rtn = LdPcAirPresCont_OnOff( LOWER, S_ON, AIR_PRES_CONT,
                                                                          PcAirPresDt.PcAirCst[cNo].pcScAir[LOWER],
                                                                          PcAirPresDt.PcAirCst[cNo].pcScAirTime[LOWER] )))
                                        {
                                            return( rtn );
                                        }
                                    }
                                    else
                                    {
                                        LdPcWtVac ( UPPER, S_ON );
                                        LdPcWtVac ( LOWER, S_ON );
                                    }
                                }
E_RETRY_SCZ:
                                if( ( rtn = LdScZGoOrg() ) )
                                {
                                    errRtn = LdErrMsg ( rtn, ERR_LOADER_RETRY,0 );
                                    if ( errRtn == ERR_RETRY_MSG )
                                    {
                                        goto E_RETRY_SCZ;
                                    }
                                    if ( errRtn == LD_DIAG_ERR )
                                    {
                                        return ( rtn );
                                    }
                                }
                                *scwaf = WAF_EMPTY;
                            }
                            else
                            {
                                /* サブチャック上にウェーハありをセット */
                                *scwaf = WAF_EXIST;
                            }
                        }
                        else
                        {
                            /* サブチャック上にウェーハありをセット */
                            *scwaf = WAF_EXIST;
                        }
                    }
                }
            }
            else
            {
                if( ( rtn = LdScVacOff( 1 ) ) )
                {
                    return( rtn );
                }

                *scwaf = WAF_EMPTY;

                /* ポインターのチェックと、ＳＲＡＭのポインターをチェック */
                sRamPtr = SramGet4( SckWaf ) ;
                if( ( OK == LdChkScWaf( WAF_EXIST, &dummyPtr ) ) ||
                    ( ( sRamPtr != 0x00 ) &&
                      ( sRamPtr != 0xff ) &&
                      ( 0 != sRamPtr ))
                  )
                {
                    /* "サブチャック上のウェーハをバキュームセンサーで検出できません。"出力 */
                    /* 状態は"ウェーハを取り除いてください。"を記述しておくこと             */
                    errRtn = LdErrMsg( SC_WAF_EXIST_CHECK,
                                       ERR_LOADER_ACK_VACCHK, 0 ) ;
                    if( ERR_RETRY_MSG == errRtn )
                    {
                        /* "イニシャル処理を続行します。ウェーハがアッパアーム上に無い事を確認してください"出力 */
                        /* 状態は"ウェーハを破損する可能性があります。"を記述しておくこと                 */
                        errRtn = LdErrMsg( SC_WAF_BREAK_WARN,
                                           ERR_LOADER_ACK_VACCHK2, 0 ) ;
                        if( ERR_RETRY_MSG == errRtn )
                        {
                            vacErrRetry = 1 ;
                        }
                        else
                        {
                            SramSet4( SckWaf, 0 ) ;
                            LdSetScWaf( WAF_EMPTY ) ;
                            break ;
                        }
                    }
                }
                else
                {
                    LdSetScWaf ( WAF_EMPTY );
                }
            }
        }while( vacErrRetry );
    }

    /* ベルヌーイ(ウェーハ上面搬送): あり */
    if( bernolliArm == 1 )
    {
        if( LdChkScWaf( WAF_EXIST, &ptrSc ) )
        {
            *scExistErr = SC_WAF_NOT_RETURN;
        }

        rtn = LdScVacOff(0);

        if( rtn == OK )
        {
            rtn = LdScZInitial();
        }

        if( rtn != OK )
        {
            return( LdSetErr(rtn, 0) );
        }
    }
    else
    {
        /* Sub-Chuck Z Recover */
        if( subchuckZMtr )
        {
            if( bernoulliWafBack == 0 )
            {
                if( scSuctionType == BERNOULLI_SC )
                {
                    LdScWtVac( S_OFF );
                }
                else
                {
                    if( ( rtn = LdScVacOff( 1 ) ) )
                    {
                        return( rtn );
                    }
                }
            }

            if( rtn == OK )
            {
                rtn = LdScZInitial();
            }

            if( rtn != OK )
            {
                return( LdSetErr(rtn, 0) );
            }
        }
    }

    /* SRAMと履歴の更新 */
    if( bernoulliWafBack == 0 )
    {
        if( ( subchuckZMtr == TRUE )
         && ( bernolliArm == FALSE ) )
        {
            /* サブチャック上にウェーハある場合 */
            if( *scwaf == WAF_EXIST )
            {
                while( 1 )
                {
                    LdSc2PcWafPtrAndSram( UPPER, &pcWafOnFlg );
                    LdSc2PcWafPtrAndSram( LOWER, &pcWafOnFlg );
                    if( pcWafOnFlg == 0 )
                    {
                        errRtn = LdErrMsg( SC_WAF_EXIST_CHECK,
                                        ERR_LOADER_ACK_VACCHK, 0 );
                        if( errRtn == ERR_RETRY_MSG )
                        {
                            errRtn = LdErrMsg( SC_WAF_BREAK_WARN,
                                       ERR_LOADER_ACK_VACCHK2, 0 );
                            if( errRtn != ERR_RETRY_MSG )
                            {
                                break ;
                            }
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                SramSet4( SckWaf, 0 );
                LdSetScWaf( WAF_EMPTY );
            }
        }
    }

    return( rtn );
}

/* @21524thos-42311 */
/*#========================================================================#*/
/*# Function    : LdRecoverPartDtDisp                                      #*/
/*# Summary     : LdPartDt Potion & Step Check                             #*/
/*# Argument    : intl mode                                                #*/
/*# Return      : void                                                     #*/
/*#========================================================================#*/
void LdRecoverPartDtDisp( intl mode )
{
    if( LD_RECOVER_DEBUG )
    {
        switch( mode )
        {
            case 0: /* ALL */
                logMsg("ALL Position Check:\n");
                logMsg("LdPartDt.uPcPos     = %d\n", LdPartDt.uPcPos );
                logMsg("LdPartDt.uPcStep    = %d\n", LdPartDt.uPcStep );
                logMsg("LdPartDt.lPcPos     = %d\n", LdPartDt.lPcPos );
                logMsg("LdPartDt.lPcStep    = %d\n", LdPartDt.lPcStep );
                logMsg("LdPartDt.pqPos      = %d\n", LdPartDt.pqPos );
                logMsg("LdPartDt.pqStep     = %d\n", LdPartDt.pqStep );
                logMsg("LdPartDt.idxPos     = %d\n", LdPartDt.idxPos );
                logMsg("LdPartDt.idxStep    = %d\n", LdPartDt.idxStep );
                logMsg("LdPartDt.scWaf      = %d\n", LdPartDt.scWaf );
                logMsg("LdPartDt.scZPos     = %d\n", LdPartDt.scZPos );
                logMsg("LdPartDt.scZStep    = %d\n", LdPartDt.scZStep );
                break;

            case 4: /* Upper Pincette */
                logMsg("Upper Pincette Position Check:\n");
                logMsg("LdPartDt.uPcPos     = %d\n", LdPartDt.uPcPos );
                logMsg("LdPartDt.uPcStep    = %d\n", LdPartDt.uPcStep );
                break;

            case 5: /* Lower Pincette */
                logMsg("Lower Pincette Position Check:\n");
                logMsg("LdPartDt.lPcPos     = %d\n", LdPartDt.lPcPos );
                logMsg("LdPartDt.lPcStep    = %d\n", LdPartDt.lPcStep );
                break;

            case 6: /* Pincette Theta */
                logMsg("Pincette Theta Position Check:\n");
                logMsg("LdPartDt.pqPos      = %d\n", LdPartDt.pqPos );
                logMsg("LdPartDt.pqStep     = %d\n", LdPartDt.pqStep );
                break;

            case 8: /* Indexer */
                logMsg("Indexer Position Check:\n");
                logMsg("LdPartDt.idxPos     = %d\n", LdPartDt.idxPos );
                logMsg("LdPartDt.idxStep    = %d\n", LdPartDt.idxStep );
                break;

            case 11: /* Sub-Chuck Z */
                logMsg("Sub-Chuck Z Position Check:\n");
                logMsg("LdPartDt.scWaf      = %d\n", LdPartDt.scWaf );
                logMsg("LdPartDt.scZPos     = %d\n", LdPartDt.scZPos );
                logMsg("LdPartDt.scZStep    = %d\n", LdPartDt.scZStep );
                break;

            default:
                break;
        }
    }
}

#endif /* P_OCTO */                                     /* @21524thos-42311 */
