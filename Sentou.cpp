#include "DxLib.h"
#include "sentou.h"
#include "GameManeger.h"
#include "Data.h"

#include <iostream>
#include <algorithm> //配列を特定の値ですべて初期化するために使用.
#include <vector>
#include <string>


//クラスはヘッダーに記載


//グローバル変数、関数
static int size;
int Chara::size = 128;//画像サイズ
int Chara::image[Chara::totalImages] = { -1 };
int MatchManager::background = 0; // backgroundの初期化
int MatchManager::image = -1;

float MatchManager::element[16][10] = { //相性表に使用 [攻撃側][防御側](個々の配列の数はキャラ数の変数にしたいかも)
    // 物理　 炎　 氷　 雷　 風　 光
      {   1,   1,   1,   1,   1,   1, 2},//プレイヤー
      {   1, 0.5,   1,   1,   2,   2, 2},//スケルトン
      { 1.5,   1,   2,   1,0.75,   2, 2},//影狼
      {0.25,   1,   1,   1,   1,   3, 2},//邪霊
      {0.75,   2,   1, 1.5, 0.5, 1.5, 2},//呪われた樹
      {0.75,   2, 0.5,   1,   1,   1, 2},//亡者の検士
      {   1,   1,   1,   1,   1,   1, 2},//教団信者
      { 0.5, 0.5,   2,0.75,   1, 1.5, 2},//腐れスライム
      {   1,   1,0.75,   1,   2, 0.5, 2},//異形の魔物
      {0.75,   2,   1, 1.5,0.75, 1.5, 2},//腐敗した触手
      {   1,0.75,   1,   2,   1, 0.5, 2},//邪神の眷属
      { 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 2},//小型ゴーレム
      { 1.5, 0.5, 0.5, 0.5, 0.5,0.25, 2},//闇の司祭
      {0.25,0.25,0.25,0.25,0.25,0.25, 2},//グラビティフリップ君
      {   3,   3,   3,   3,   3,   3, 2} 
};


enum Action
{
    ACTION_ATTACK = 0,//こうげき
    ACTION_SKILLATTACK = 1,//とくぎでこうげき
    ACTION_SKILL = 2,//とくぎ
    ACTION_TOOLS = 3,//どうぐ
    ACITON_DEFENSE = 4,//防御
    ACITON_ESCAPE = 5//逃げる
};


bool Sentouflg;//現在戦闘中かどうかのフラグ.
bool fasttime = true;//本当の初回起動時に実行
int Itemnum[256][2] = { 0 };//Id, 個数
  

std::vector<Chara> Charas;//キャラオブジェクトの配列.
std::vector<std::vector<int>> numbers(1, std::vector<int>(8));//キャラのデータを配列に入れるために整理する箱.

std::vector<std::vector<int>> atkpara;//攻撃力をこの二時限配列に入れる
std::vector<std::vector<int>> elementpara;//属性をこの二時限配列に入れる

std::vector<std::vector<int>> usemppoint;//消費MPをこの二時限配列に入れる

std::vector<std::vector<bool>> skillpara;//全体攻撃か否かこの二時限配列に入れる
std::vector<std::vector<std::string>> commandpara;//コマンドをこの二時限配列に入れる

std::vector<std::string> loghistory;//戦闘の状況を解説するログ用の配列

//敵は下記の配列に入れることで生成
std::vector<int> Enemys = {};//7.8.9.10



//クラスの作成
MatchManager Manager;
CommandSelector C_Selecter;
InputManager InputHandler;

//戦闘に関するデータをすべてリセット
void allDelete()
{    
    Charas.clear();
    numbers.clear();
    atkpara.clear();
    usemppoint.clear();
    skillpara.clear();
    elementpara.clear();
    commandpara.clear();
    loghistory.clear();
    Enemys.clear();

    //コンストラクタが呼び出してリセットする.
    Manager = MatchManager(); 
    C_Selecter = CommandSelector(); 
    InputHandler = InputManager();

    Sentouflg = true;
}

//戦闘が終わった
void endGame(int number)
{
    if (number == 1)//勝ち
    {
        player.BattleEnd(Charas[0].getHp(), Charas[0].getMp());
        for (int i = 0; i < Enemys.size(); i++)
        {
            if (player.GetExp(Enemys[i]))
            {
                Manager.setLevelUpFlg(TRUE);
            }
            player.SetGold(Enemys[i]);
            Manager.addAllExp(enemy[Enemys[i]].GetExp());
            Manager.addAllGold(enemy[Enemys[i]].GetGold());

            if (Manager.getWeaponId() == 0)
            {
                Manager.setWeaponId(player.Drop(Enemys[i]));
            }
        }


    }
    //負け
    else if (number == -1)
    {
        player.BattleEnd(1, Charas[0].getMp());
    }
    else if (number == 0)
    {
        player.BattleEnd(Charas[0].getHp(), Charas[0].getMp());

    }

    int cnt = 0;
    while (Itemnum[cnt][1] != 0)
    {
        cnt++;
    }

    Manager.setGameFlg(false);
    Manager.setWinnerflg(number);
}

//敵キャラのセット
void setEnemys(int number) { Enemys.push_back({ number + 1 }); }



void sentou()
{
    //初回起動時に必要
    if (fasttime)
    {
       // printfDx("初回起動時の読み込み\n");
        Chara::LoadImage("主人公2.png");//別のとこで一回きりの読み込みにしたい

        Manager.LoadImage("textwindow.png");//枠の保存

        Manager.LoadImageBackground("sougen_back.png");

        fasttime = false;

    }


    if (Sentouflg)//戦闘フラグが真なら.
    {
        //各種設定の初期化
        int kankaku = 1;
        int aaaaa = 1;


        int Tekisan[3] = { 0 };
        int AllEnemyID = 0;
        int tekisuu = player.GetLevel() / 5;
        if (tekisuu > 2) tekisuu = 2;

        int tekiID = player.GetLevel();
        if (player.GetLevel() == 0) tekiID = 1;
        if (player.GetLevel() >= 11) tekiID = 11;

        
        for (int i = 0; i < 1 + tekisuu; i++)
        {
             AllEnemyID = Tekisan[0] + Tekisan[1] + Tekisan[2];
            if (i == 0)
            {
                Tekisan[0] = GetRand(tekiID);
                setEnemys(Tekisan[0]);
             
            }
            else if (i == 1 && tekiID - AllEnemyID- 1 > 0 && tekiID - AllEnemyID > 3)
            {
                Tekisan[1] = GetRand(tekiID - AllEnemyID);
                setEnemys(Tekisan[1]);
            
            }
            else if (i == 2 && tekiID - AllEnemyID  - 1> 0 && tekiID - AllEnemyID > 3  && Tekisan[1] != 0)
            {
                Tekisan[2] = GetRand(tekiID - AllEnemyID);
                setEnemys(Tekisan[2]);
            }
        }
        

     


        for (int j = 0; j < 1; j++)
        {
            //プレイヤーの数ループ(今回は一体のみだからいいよ)
            numbers.push_back(std::vector<int>{player.GetId(), player.GetMaxHp(), player.GetMaxMp(), player.GetStr(), player.GetDef(), player.GetAgi(), player.GetLevel(), player.GetId()});//番号,hp,mp,atk,def,spd

            //プレイヤーの情報を保存する場所を確保
            commandpara.push_back({ });
            atkpara.push_back({ });
            usemppoint.push_back({ });
            elementpara.push_back({ });
            skillpara.push_back({ });

            for (int i = 1; i <= player.GetSkillCnt() - 1; i++)
            {
                // printfDx("スキル%d:%s\n", i, skill[i].GetName().c_str());

               //  printfDx("威力%d:%d\n", i, skill[i].GetDmg());

                 //コマンドの情報(技名)
                commandpara[commandpara.size() - 1].push_back({ skill[i].GetName() });

                //コマンドの情報(属性)
                elementpara[elementpara.size() - 1].push_back({ skill[i].GetEle() });

                //コマンドの情報(攻撃力)
                atkpara[atkpara.size() - 1].push_back({ skill[i].GetDmg() });

                //コマンドの情報(消費MP)
                usemppoint[usemppoint.size() - 1].push_back({ skill[i].GetMp() });

                //全体攻撃か否か(真なら全体攻撃)
                skillpara[skillpara.size() - 1].push_back({ skill[i].GetFlg() });

            }


        }


        //敵はMPを消費しないけどフォーマットに合わせるために作成
        usemppoint.push_back({ 0,0,0,0 });

        int i = 0;
        for (int j = 0; j < Enemys.size(); j++)
        {//番号,hp,mp,atk,def,spd,level, element(新規追加)

            numbers.push_back(std::vector<int>{enemy[Enemys[i]].GetId(), enemy[Enemys[i]].GetHp(), enemy[Enemys[i]].GetMp(), enemy[Enemys[i]].GetStr(), enemy[Enemys[i]].GetDef(), enemy[Enemys[i]].GetAgi(), enemy[Enemys[i]].GetLevel(), enemy[Enemys[i]].GetId()});//番号,hp,mp,atk,def,spd

            //コマンド名の取得
            commandpara.push_back({ enemy[Enemys[i]].GetSkillName(0), enemy[Enemys[i]].GetSkillName(1), enemy[Enemys[i]].GetSkillName(2), enemy[Enemys[i]].GetSkillName(3) });   // 文字列の配列

            //技の威力の取得
            atkpara.push_back({ enemy[Enemys[i]].GetSkillDmg(0) , enemy[Enemys[i]].GetSkillDmg(1), enemy[Enemys[i]].GetSkillDmg(2), enemy[Enemys[i]].GetSkillDmg(3) });

            //属性の取得
            elementpara.push_back({ enemy[Enemys[i]].GetSkillEle(0) , enemy[Enemys[i]].GetSkillEle(1), enemy[Enemys[i]].GetSkillEle(2), enemy[Enemys[i]].GetSkillEle(3) });

            //全体攻撃か否か(真なら全体攻撃)
            skillpara.push_back({ enemy[Enemys[i]].GetSkillFlg(0) , enemy[Enemys[i]].GetSkillFlg(1), enemy[Enemys[i]].GetSkillFlg(2), enemy[Enemys[i]].GetSkillFlg(3) });

            i++;
        }



        int cnt = 0;
        for (int i = 0; i < 256; i++)
        {
            //printfDx("%d番目のID%d\n", i, item[i].GetId());

            for (int j = 0; j < 2; j++)
            {
                Itemnum[i][j] = player.GetPosItem(i, j);
            }
            if (Itemnum[i][0] == 0)
            {
                break;
            }

            cnt++;
        }


        //アイテムの情報を保存する場所を確保
        commandpara.push_back({});
        atkpara.push_back({ });
        elementpara.push_back({ });
        //アイテムはMPを消費しないけどフォーマットに合わせるために作成
        usemppoint.push_back({  });
        skillpara.push_back({ });

        for (int i = 0; i < cnt; i++)
        {
            //アイテム情報(名前)
            commandpara[commandpara.size() - 1].push_back({ item[Itemnum[i][0] - 2000].GetName() });

            //アイテム情報(攻撃力)
            atkpara[atkpara.size() - 1].push_back({ -item[Itemnum[i][0] - 2000].GetHp() });

            //アイテムの情報(属性)
            elementpara[elementpara.size() - 1].push_back({ 0 });

            //下記2つは使用しないけど埋めないといけない
            usemppoint[usemppoint.size() - 1].push_back({ 0 });
            skillpara[skillpara.size() - 1].push_back({ 0 });


        }


        //アイテムの回復は固定値、敵を倒すと配列外に(右から二番目)MPの導入、HP、最大HPを分ける。


        //固定のコマンド
        commandpara.push_back({ "こうげき", "とくぎ", "どうぐ", "ぼうぎょ", "にげる" });//けつに基本のコマンド


        C_Selecter.setWhichCharacters(commandpara.size() - 1);//けつのコマンドをセット(コマンドセレクトのことね)


        //配列のデータをもとにキャラクターの生成.
        for (size_t i = 0; i < numbers.size(); ++i) {
            if (numbers[i][6] != 0)//レベルが0でないなら(プレイヤーなら).
            {
                Charas.push_back(Chara(100 + (Manager.getPlayerCnt() * Chara::size), 400, numbers[i][0], numbers[i][1], numbers[i][2], numbers[i][3], numbers[i][4], numbers[i][5], numbers[i][6], numbers[i][7], i));
                Manager.addPlayerCnt();
                C_Selecter.setCharaList(i, -1);//プレイヤーの番号を保存

                //ここで現在のHP、MPを適用
                Charas[i].setHp(player.GetHp());
                Charas[i].setMp(player.GetMp());
                // player.SetHp()
            }
            else if (numbers[i][6] == 0)//エネミ－なら
            {
                Charas.push_back(Chara(150 + (Manager.getEnemyCnt() * Chara::size), 100, numbers[i][0], numbers[i][1], numbers[i][2], numbers[i][3], numbers[i][4], numbers[i][5], numbers[i][6], numbers[i][7], i));
                Manager.addEnemyCnt();
                C_Selecter.setCharaList(-1, i);//エネミーの番号を保存
            }

        }


        for (auto& c : Charas)//配列に素早さ、番号だけで保存
        {
            Manager.setSpdAndOrder(c.getSpd(), c.getOrder());
        }

        //1回読み込めたらフラグを変更        
        Sentouflg = false;

        Manager.addAnyNumber();



        loghistory.insert(loghistory.begin(), "が現れた！");//配列の先頭のアドレスを指定し、戦闘に要素を追加

        while (Manager.getAnyNumber() <= Manager.getEnemyCnt() && Manager.getAnyNumber() >= 0)

            //if (Manager.Stay(20) && Manager.getAnyNumber() <= Manager.getEnemyCnt() && Manager.getAnyNumber() >= 0)
        {
            int j = Manager.getAnyNumber();
            if (j < Manager.getEnemyCnt())
            {
                //名前の表示
                loghistory.insert(loghistory.begin(), enemy[Charas[C_Selecter.getCharaList(1, j)].getNumber()].GetName() + "");//配列の先頭のアドレスを指定し、戦闘に要素を追加
            }
            else
            {
            }

            Manager.addAnyNumber();


        }

        //敵の出現ログの表示

    }
    
    
 
   


    
    if (!Manager.getGameFlg())//勝敗が決まったならなら
    {
        //欲を言うとボタンを押したら進むようにしたいね。
        //戦闘中のログはオートでいいかな
        int number = 0;
        int StayTime = 35;
        
      
        if (Manager.Stay(StayTime))
        {
            while (Manager.getLogNumberFlg(number) == TRUE   &&  Manager.getAnyFlg() == FALSE)
            {
                number++;
            }
        }

    
        if (Manager.getLogNumberFlg(number) == false)
        {
   
            Manager.setLogNumberFlg(number, TRUE);
            switch (number)
            {
            case 0:
                if (Manager.getWinnerFlg() == 1)//勝ち
                {
                    
                    loghistory.insert(loghistory.begin(), player.GetName() + "の勝利！！");
                }
                else if (Manager.getWinnerFlg() == -1)//まけ
                {
                    loghistory.insert(loghistory.begin(), "全滅してしまった...");
                    
                }
                else if (Manager.getWinnerFlg() == 0)
                {
                    loghistory.insert(loghistory.begin(), player.GetName() + "は逃げた...");

                }
                else
                {
                    loghistory.insert(loghistory.begin(), "例外が発せせせせせせせせせせせせ");
                }
               
                
                break;
            case 1:
                
                if (Manager.getWinnerFlg() == 1)//勝ち
                {
               

                    loghistory.insert(loghistory.begin(), "経験値" + std::to_string(Manager.getAllExp()) + "獲得!");
                }
                else if (Manager.getWinnerFlg() == -1)//まけ
                {
                    loghistory.insert(loghistory.begin(), "目の前が真っ暗になった...");
                   
                }
                else if (Manager.getWinnerFlg() == 0)
                {
                    loghistory.insert(loghistory.begin(), "スペースキーで戦闘の終了");
                    Manager.setAnyFlg(TRUE);
                }
                else
                {
                    loghistory.insert(loghistory.begin(), "例外が発せせせせせせせせせせせせ");
                }
                break;

            case 2:
                if (Manager.getWinnerFlg() == 1)//勝ち
                {
                    if (Manager.getLevelUpFlg() == true)
                    {
                        loghistory.insert(loghistory.begin(), player.GetName() + "はレベルが上がった！");
                         break;
                    }
                    else
                    {
                        number++;
                        Manager.setLogNumberFlg(number, TRUE);
                    }
                }
                else if (Manager.getWinnerFlg() == -1)//まけ
                {
                    loghistory.insert(loghistory.begin(), "スペースキーで戦闘の終了");
                    Manager.setAnyFlg(TRUE);
                    break;
                }
                else
                {
                    loghistory.insert(loghistory.begin(), "例外が発せせせせせせせせせせせせ");
                    break;
                }

             
            case 3:
                if (Manager.getWinnerFlg() == 1)//勝ち
                {
                    loghistory.insert(loghistory.begin(), std::to_string(Manager.getAllGold()) + "ゴールドの獲得!");
                  
                }
                else if (Manager.getWinnerFlg() == -1)//まけ
                {

                    loghistory.insert(loghistory.begin(), "スペースキーで戦闘の終了");
                    Manager.setAnyFlg(TRUE);
                }
                else
                {
                    loghistory.insert(loghistory.begin(), "例外が発せせせせせせせせせせせせ");
                }
                break;
            case 4:
                if (Manager.getWinnerFlg() == 1)//勝ち
                {
                    if (Manager.getWeaponId() == 0 )
                    {
                        number++;
                        Manager.setLogNumberFlg(number, TRUE);
                        
                    }
                    else
                    {
                        loghistory.insert(loghistory.begin(), "装備品、" + soubi[Manager.getWeaponId()].GetName() + "の獲得!");
                        break;
                    }
                }
                else if (Manager.getWinnerFlg() == -1)//まけ
                {
                    allDelete();
                    break;
                }
                else
                {
                    loghistory.insert(loghistory.begin(), "例外が発せせせせせせせせせせせせ");
                    break;
                }
                
            case 5:
                loghistory.insert(loghistory.begin(), "スペースキーで戦闘を終了します！");
                Manager.setAnyFlg(TRUE);
                break;
            case 6:
                allDelete();
                break;
            default:
                printfDx("~ERROR~本来この分は出まmmmmmmmmmmmmmmmmmmmmmm");
                break;
            }
        }

        if (Manager.getAnyFlg() == true)
        {
            DrawString(935, 600, "Spaceで戦闘の終了", 0xffffff);
        }
        
        //勝ち負けが決まっていたら戦闘を終了できるようにする(将来はここで戦闘フラグをリセット)
        if (InputHandler.CheackAnyKeyDown(KEY_INPUT_SPACE) && Manager.getAnyFlg() == TRUE)
        {
            allDelete();

            //changeCurrentState(TITLE);
            changeCurrentState(GAME);

        }
     
        
    }




    //戦闘の管理
    Manager.DrawBackGround(0,100,1400,600);//草原の背景の表示
    
    Manager.addTurn();//行動が一周したらターンを++(イラン)
    Manager.orderSelect();//素早さ順でデータを並び替え
    Manager.DrawOnlyFrame(925, 425, 1220, 550);//コマンドフレームの描画
    Manager.DrawOnlyFrame(925, 50, 1270, 350);//ログの枠の描画
    


    
    //一周したなら
    if (Manager.getCurrentTurn() > Charas.size() - 1)//CurentTurnは0から要素が入っているので、-1してずらす.
    {
        Manager.setCurrentTurn(0);
    }


    if (Charas.size())//配列に要素が入っているなら
    {
        //枠の描画
        Manager.DrawFrame(Charas.front().getX(), Charas.front().getY());//左端X、Y

        
    }




    //ログを描画するタイミング。Stay()関数を使う
    //戦闘終了を作る。アイテム獲得のタイミング
    //たまにコマンドを選択したら配列外にアクセスしちゃう

    //ログの描画
    for (int i = 0; i < 10; i++)
        //for (int i = 0; i < loghistory.size(); i++)
    {

        if (loghistory.size() > i)
        {
            DrawString(950, 100 + i * 20, loghistory[i].c_str(), 0xffffff);

        }
    }
    

    //敵の攻撃処理
    if (Manager.getEnemyFlg() == true)
    {
        //少し待ってから攻撃
        if (Manager.Stay(30) && Manager.getGameFlg())
        {
            //どのプレイヤーを攻撃するかの抽選を後程行う
            int atknumber;

            bool target = 0;//0がプレイヤー、1がエネミー.

            if (Manager.getSkillAtk() >= 0)
            {
                atknumber = GetRand(C_Selecter.getSizeReserve(0));//ここでしっかり範囲を定める
                target = 0;
                int cnt = 0;
                if (Manager.getSkillFlg())//全体攻撃かどうかの抽選
                {

                    int ave = 0;
                    int cnt = 0;
                    for (int i = 0; i <= C_Selecter.getSizeReserve(0); i++)
                    {
                        
                        cnt++;
                        Manager.setDef(Charas[C_Selecter.getCharaList(target, i)].getDef(),
                            Charas[C_Selecter.getCharaList(target, i)].getElement());

                        int dmg = Manager.Atack(C_Selecter.getEnyrAction());
                        Charas[C_Selecter.getCharaList(target, i)].setHp(Charas[C_Selecter.getCharaList(target, i)].getHp() - dmg);


                        ave = (ave * (cnt - 1) + dmg) / cnt;
                    }
                    loghistory.insert(loghistory.begin(), "平均" + std::to_string(ave) + "のダメージ！");
                    for (int i = 0; i <= C_Selecter.getSizeReserve(0); i++)
                    {
                        if (Charas[C_Selecter.getCharaList(target, i)].getHp() <= 0 && Charas[C_Selecter.getCharaList(target, i)].getDeathFlg() == false)
                        {
                            loghistory.insert(loghistory.begin(), player.GetName() + "は倒れた!");
                            Charas[C_Selecter.getCharaList(target, i)].setDeathFlg(TRUE);
                        }
                    }

                }
                else //単体攻撃なら
                {

                    while ((Charas[C_Selecter.getCharaList(target, atknumber)].getHp() <= 0 || Charas[C_Selecter.getCharaList(target, atknumber)].getLevel() == 0) && cnt < 1000)//体力が0以上でプレイヤーなら(この場合はレベル0は味方)
                    {
                        cnt++;
                        //エネミーと同じ関数に
                        atknumber = GetRand(C_Selecter.getSizeReserve(0));
                    }

                    Manager.setDef(Charas[C_Selecter.getCharaList(target, atknumber)].getDef(),
                        Charas[C_Selecter.getCharaList(target, atknumber)].getElement());

                    int dmg = Manager.Atack(C_Selecter.getEnyrAction());
                    Charas[C_Selecter.getCharaList(target, atknumber)].setHp(Charas[C_Selecter.getCharaList(target, atknumber)].getHp() - dmg);
                    loghistory.insert(loghistory.begin(), player.GetName() + "に" + std::to_string(dmg) + "のダメージ！");
                    if (Charas[C_Selecter.getCharaList(target, atknumber)].getHp() <= 0 && Charas[C_Selecter.getCharaList(target, atknumber)].getDeathFlg() == false)
                    {
                        loghistory.insert(loghistory.begin(), player.GetName() + "は倒れた!");
                        Charas[C_Selecter.getCharaList(target, atknumber)].setDeathFlg(TRUE);

                    }

                }

                Manager.risetAtackData();
            }
            else //回復
            {
                target = 1;//標的を味方に              
  
                int Rand = GetRand(99);
                //70%の確率で弱っている味方を回復
                if (Rand <= 70)
                {
                    int EnemyDate[2] = { 0,9999 };//id,HP
                    for (int i = 0; i <= C_Selecter.getSizeReserve(1); i++)
                    {
                        if (EnemyDate[1] > Charas[C_Selecter.getCharaList(target, i)].getHp() && Charas[C_Selecter.getCharaList(target, i)].getHp() >0)
                        {
                            EnemyDate[0] = i;
                            EnemyDate[1] = Charas[C_Selecter.getCharaList(target, i)].getHp();     
                        }
                    }
                    
                    Manager.setDef(Charas[C_Selecter.getCharaList(target, EnemyDate[0])].getDef(),
                        Charas[C_Selecter.getCharaList(target, EnemyDate[0])].getElement());

                    int dmg = Manager.Atack(C_Selecter.getEnyrAction());
                    Charas[C_Selecter.getCharaList(target, EnemyDate[0])].setHp(Charas[C_Selecter.getCharaList(target, EnemyDate[0])].getHp() - dmg);
                    loghistory.insert(loghistory.begin(), enemy[Charas[C_Selecter.getCharaList(target, EnemyDate[0])].getNumber()].GetName() + "を" + std::to_string(dmg *= -1) + "回復！");
                }
                else
                {
                    atknumber = GetRand(C_Selecter.getSizeReserve(1));//ここでしっかり範囲を定める

                    int cnt = 0;

                    while ((Charas[C_Selecter.getCharaList(target, atknumber)].getHp() <= 0 || Charas[C_Selecter.getCharaList(target, atknumber)].getLevel() != 0) && cnt < 1000)
                    {
                        cnt++;
                        atknumber = GetRand(C_Selecter.getSizeReserve(1));
                    }
                    Manager.setDef(Charas[C_Selecter.getCharaList(target, atknumber)].getDef(),
                        Charas[C_Selecter.getCharaList(target, atknumber)].getElement());


                    int dmg = Manager.Atack(C_Selecter.getEnyrAction());

                    Charas[C_Selecter.getCharaList(target, atknumber)].setHp(Charas[C_Selecter.getCharaList(target, atknumber)].getHp() - dmg);

                    loghistory.insert(loghistory.begin(), enemy[Charas[C_Selecter.getCharaList(target, atknumber)].getNumber()].GetName() + "を" + std::to_string(dmg *= -1) + "回復！");

                }
                
              
            }
            //攻撃先をわかりやすくするやつ
            /*
            DrawBox(Charas[C_Selecter.getCharaList(target, atknumber)].getX(), Charas[C_Selecter.getCharaList(target, atknumber)].getY(),
                Charas[C_Selecter.getCharaList(target, atknumber)].getX() + Chara::size, Charas[C_Selecter.getCharaList(target, atknumber)].getY() + Chara::size * 2, 0xff22f0, false);
                */
            //全滅したらゲーム終了
            if (C_Selecter.checkBattleEnds(target, Charas))
            {
                endGame(-1);
            }

            //この辺に文字の描画もしたいよね

            
            
                Manager.setEnemyFlg(false);
                Manager.addCurrentTurn();
            
        }


    }


     if (C_Selecter.getSelecter() == 1 && Manager.getSkillFlg() == false)//敵を選ぶフラグが真なら(個々の前でコマンドの決定、攻撃力の送信は住んでいる)
    {
        bool target = 1;//0がプレイヤー、1がエネミー.
        DrawString(935, 600, "A/Dで選択、Spaceで決定", 0xffffff);

        
        if (Manager.getItemType(0) > 0 || Manager.getItemType(1) > 0 || Manager.getSkillAtk()  < 0)
        {
            target = 0;
            //カーソルが敵から味方になるので、生きている見方を探索(一回限り実行)
            if (Manager.fastSet())
            {
                int i = 0;
                while (Charas[C_Selecter.getCharaList(target, i)].getHp() == 0)
                {
                    i++;
                }
                C_Selecter.setCursorPos(i);
            }
        }

        if (Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].getHp() == 0)
        {
          //  printfDx("死んだ敵にあってますやん\n");
            //カーソルが敵から味方になるので、生きている見方を探索(一回限り実行)
            if (Manager.fastSet())
            {
                int i = 0;
                while (Charas[C_Selecter.getCharaList(target, i)].getHp() == 0)
                {
                    i++;
                }
                C_Selecter.setCursorPos(i);
            }
        }


        //全体攻撃でないならカーゾルの操作できる(上で検証済み)
        //ボタンが押されてカーソルが一番左にいないなら.
        if (InputHandler.CheackAnyKeyDown(KEY_INPUT_A) && C_Selecter.getCursorPos() > 0)
        {//隣が死んでるなら飛ばそう.
            if (Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos() - 1)].getHp() == 0)
            {
                C_Selecter.checkCanShift(target, -1, Charas);
            }
            else
            {
                C_Selecter.setCursorPos(C_Selecter.getCursorPos() - 1);
            }
        }

        if (InputHandler.CheackAnyKeyDown(KEY_INPUT_D) && C_Selecter.chackEnemyList(target, C_Selecter.getCursorPos()))
        {
            //隣が死んでるなら飛ばそう.
            if (Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos() + 1)].getHp() == 0)
            {
                C_Selecter.checkCanShift(target, 1, Charas);
            }
            else//生きているなら一つずらす
            {
                C_Selecter.setCursorPos(C_Selecter.getCursorPos() + 1);
            }
        }

        if (InputHandler.CheackAnyKeyDown(KEY_INPUT_SPACE))
        {

             //回復道具なら
            if (Manager.getItemType(0) > 0)
            {
               
                Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].setHp(Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].getHp() + Manager.getItemType(0));                
                loghistory.insert(loghistory.begin(), player.GetName() + "のHPを" + std::to_string(Manager.getItemType(0)) + "回復！");


            }
            //MP回復道具なら
            else if (Manager.getItemType(1) > 0)
            {
                Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].setMp(Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].getMp() + Manager.getItemType(1));
                loghistory.insert(loghistory.begin(), player.GetName() + "のMPを" + std::to_string(Manager.getItemType(1)) + "回復！");
            }
            else
            {
                
                
                //体力が0ならターンを飛ばしたいな
               //攻撃を受ける側の防御力,属性を送信
                Manager.setDef(Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].getDef(), Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].getElement());
                //ここで攻撃!
                int dmg = Manager.Atack(C_Selecter.getEnyrAction());
                Manager.risetAtackData();//attackのデータリセット
                Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].setHp(Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].getHp() - dmg);

                //少し弱気にする
                Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].setEnemyFeeling(-20);
               
                //ログ
                if (dmg > 0)
                {
                    loghistory.insert(loghistory.begin(), enemy[Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].getNumber()].GetName() + "に" + std::to_string(dmg) + "のダメージ！");
                }
                else
                {
                    loghistory.insert(loghistory.begin(), player.GetName() + "のHPを" + std::to_string(-dmg) + "回復！");

                }
                

                //全滅したらゲーム終了
                if (C_Selecter.checkBattleEnds(target, Charas))
                {
                    endGame(1);

                }
                //敵が死んだら
                else if (Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].getHp() == 0)
                {
                    //カーソルを生きている敵にずらす
                    int i = 0;
                    while (Charas[C_Selecter.getCharaList(target, i)].getHp() == 0)
                    {
                        i++;
                    }
                    C_Selecter.setCursorPos(i);
                }

            }
            Manager.setItemType(0, 0);
            Manager.risetFastSet(true);

            C_Selecter.setSelecter(0);//フラグをfalseに 
            Manager.addCurrentTurn();//攻撃したのでターンを回す            
        }



        //ここの枠を関数にしたいな
        /*
        DrawString(Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].getX(), Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].getY(),
            std::to_string(C_Selecter.getCursorPos()).c_str(), 0xff0fff);

        DrawString(0, 400, std::to_string(C_Selecter.getCursorPos()).c_str(), 0xff0fff);
        */
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 54); // アルファブレンドモードを設定、透明度128
        
        DrawBox(Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].getX(), Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].getY(),
            Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].getX() + Chara::size, Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].getY() + Chara::size * 2, 0xff0000, true);
    
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); // 描画後にブレンドモードを戻す

}
     else if (C_Selecter.getSelecter() == 1 && Manager.getSkillFlg() == true)//全体攻撃
     {
         bool target = 1;//0がプレイヤー、1がエネミー.
         if (Manager.getSkillAtk() < 0)
         {
             target = 0;
             //カーソルが敵から味方になるので、生きている見方を探索(一回限り実行)
             if (Manager.fastSet())
             {
                 int i = 0;
                 while (Charas[C_Selecter.getCharaList(target, i)].getHp() == 0)
                 {
                     i++;
                 }
                 C_Selecter.setCursorPos(i);
             }
         }

         int ave = 0;
         int cnt = 0;
         for (int i = 0; i <= C_Selecter.getSizeReserve(target); i++)
         {
             cnt++;

             //体力が0ならターンを飛ばしたいな
             //攻撃を受ける側の防御力,属性を送信
             Manager.setDef(Charas[C_Selecter.getCharaList(target, i)].getDef(), Charas[C_Selecter.getCharaList(target, i)].getElement());
             //ここで攻撃!
             int dmg = Manager.Atack(C_Selecter.getEnyrAction());
             Charas[C_Selecter.getCharaList(target, i)].setHp(Charas[C_Selecter.getCharaList(target, i)].getHp() - dmg);
          
             ave = (ave * (cnt - 1) + dmg) / cnt;
         

             //すべての敵を弱気に
             Charas[C_Selecter.getCharaList(target, i)].setEnemyFeeling(-10);
         }
         if (target == 1)
         {
             loghistory.insert(loghistory.begin(), "平均" + std::to_string(ave) + "のダメージ！");
         }
         else
         {
             loghistory.insert(loghistory.begin(), "平均" + std::to_string(ave *= -1) + "の回復！");
         }
         Manager.risetAtackData();//attackのデータリセット


         //全滅したらゲーム終了
         if (C_Selecter.checkBattleEnds(target, Charas))
         {
             endGame(1);
             
         }
         //敵が死んだら
         else if (Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].getHp() == 0)
         {
             //カーソルを生きている敵にずらす
             int i = 0;
             while (Charas[C_Selecter.getCharaList(target, i)].getHp() == 0)
             {
                 i++;
             }
             C_Selecter.setCursorPos(i);
         }

         Manager.risetFastSet(true);
         Manager.setSkillFlg(false);
         C_Selecter.setSelecter(0);//フラグをfalseに
         Manager.addCurrentTurn();//攻撃したのでターンを回す
     }

     //キャラの動き、描画.
     for (auto& c : Charas) {




         //プレイヤーの描画
         if (c.getLevel() != 0)
         {
             c.Draw();
             c.DrawData(1);//ステータスの描画(1が味方、0が敵).
         }
         else
         {
             bool flg = (c.getHp() != 0);

             //後ろにbool型を追加
             enemy[c.getNumber()].DrowEnemy(c.getX(), c.getY(), c.getX() + c.getSize(), c.getY() + c.getSize(), flg);

             c.DrawData(0);//ステータスの描画(1が味方、0が敵).
         }


         if (Manager.getGameFlg())//ゲームが進行中なら
         {

             //自身のターンなら(番号が一致かつ、攻撃先を選ぶターンでないなら)
             if (c.getOrder() == Manager.getOrder(Manager.getCurrentTurn()) && C_Selecter.getSelecter() == 0)
             {
                 DrawString(935, 600, "W/Sで選択、Spaceで決定、Qで戻る", 0xffffff);

                 if (c.getDefenseFlg() == true)
                 {
                     c.setgetDefenseFlg(false);
                     loghistory.insert(loghistory.begin(), player.GetName() + "は防御をといた");

                 }

                 if (c.getHp() <= 0)//体力がないなら次の番に
                 {

                     Manager.addCurrentTurn();
                     continue;
                 }
                 //ターンが来た！.コマンドの選択      
                 DrawBox(c.getX(), c.getY(), c.getX() + Chara::size, c.getY() + Chara::size * 2, 0xfff000, false);


                 if (c.getLevel() != 0)//プレイヤーならコマンド選択選択
                 {
                     if (InputHandler.CheackAnyKeyDown(KEY_INPUT_W))
                     {
                         //カーソルを上に.(上限の追加)
                         C_Selecter.UpdateCursor(-1, C_Selecter.getArraySize(commandpara, C_Selecter.getWhichCharacters()));
                     }

                     if (InputHandler.CheackAnyKeyDown(KEY_INPUT_S))
                     {
                         //カーソルを下に.             
                         C_Selecter.UpdateCursor(1, C_Selecter.getArraySize(commandpara, C_Selecter.getWhichCharacters()));
                     }

                     /*
                     if (InputHandler.CheackAnyKeyHold(KEY_INPUT_N))
                     {
                         DrawString(0, 300, std::to_string(C_Selecter.getArraySize(commandpara, C_Selecter.getWhichCharacters())).c_str(), 0xff0fff);
                         DrawString(0, 330, commandpara[C_Selecter.getWhichCharacters()][C_Selecter.getCommandPos()].c_str(), 0xffffff);
                         Manager.addCurrentTurn();//攻撃したのでターンを回す
                     }*/

                     if (InputHandler.CheackAnyKeyHold(KEY_INPUT_Q))//もとのコマンドに戻る
                     {
                         C_Selecter.UpdateCursor(0, C_Selecter.getArraySize(commandpara, c.getOrder()));//カーソルを一番上に
                         C_Selecter.setWhichCharacters(commandpara.size() - 1);//リセット
                     }

                     if (InputHandler.CheackAnyKeyDown(KEY_INPUT_SPACE))
                     {
                         //全体に向けた行動ができない、死ぬと強くなって再スタート


                         //ここで選択したコマンドが攻撃なのか、別のコマンドに繊維なのか判別する(関数終わってる)
                         int action = C_Selecter.checkStringOrInt(C_Selecter.getWhichCharacters(), C_Selecter.getCommandPos(), commandpara);

                         switch (action)
                         {
                         case ACTION_ATTACK://攻撃(とくぎでない)

                             //選択したコマンドの攻撃力、属性を取得
                            // Manager.setAtk(c.getAtk(), 0, elementpara[c.getOrder()][C_Selecter.getCommandPos()]);//攻撃力、skillの威力、技属性
                             Manager.setAtk(c.getAtk(), 0, 0, 0);//攻撃力、skillの威力、技属性
                             C_Selecter.setWhichCharacters(commandpara.size() - 1);//リセット
                             C_Selecter.setSelecter(1);//敵を選ぶフラグを真にする.

                             //ログ
                             loghistory.insert(loghistory.begin(), player.GetName() + "のこうげき！");


                             break;

                         case ACTION_SKILLATTACK://とくぎのこうげき,道具の使用

                             //道具なら
                             if (C_Selecter.getWhichCharacters() == commandpara.size() - 2)
                             {

                                 //ここでは道具にしかアクセスしないので、第一引数に道具の住所を教えています。
                                 //可能ならスキルと合わせたいね。

                                 //選択したコマンドの攻撃力、属性を取得
                                // Manager.setAtk(c.getAtk(), atkpara[C_Selecter.getOrderNumber()][C_Selecter.getCommandPos()], elementpara[C_Selecter.getOrderNumber()][C_Selecter.getCommandPos()], skillpara[c.getOrder()][C_Selecter.getCommandPos()]);//攻撃力、skillの威力、技属性
                                 Manager.setAtk(0, atkpara[commandpara.size() - 2][C_Selecter.getCommandPos()], 0, skillpara[commandpara.size() - 2][C_Selecter.getCommandPos()]);//攻撃力、skillの威力、技属性,全体攻撃かどうか
                                 //HP,MPの回復か判別するために保存しとく


                                 //下記だとMPがなんかダメ
                                 Manager.setItemType(item[Itemnum[C_Selecter.getCommandPos()][0] - 2000].GetHp(), item[Itemnum[C_Selecter.getCommandPos()][0] - 2000].GetMp());


                                 //Dataのほうに使用したidを送信
                                 player.DecItem(Itemnum[C_Selecter.getCommandPos()][0], 1);


                                 /*
                                 printfDx("名前:%s\n", item[Itemnum[C_Selecter.getCommandPos()][0] - 2000].GetName().c_str());
                                 printfDx("消費HP:%d\n", item[Itemnum[C_Selecter.getCommandPos()][0] - 2000].GetHp());
                                 printfDx("消費MP:%d\n", item[Itemnum[C_Selecter.getCommandPos()][0] - 2000].GetMp());
                                 */


                                 loghistory.insert(loghistory.begin(), player.GetName() + "の" + commandpara[commandpara.size() - 2][C_Selecter.getCommandPos()].c_str() + "!");


                                 //ここで個数をマイナス
                                 Itemnum[C_Selecter.getCommandPos()][1] = Itemnum[C_Selecter.getCommandPos()][1] - 1;


                                 //もし個数が0になったら。
                                 if (Itemnum[C_Selecter.getCommandPos()][1] == 0)
                                 {
                                     // printfDx("使い切っちゃった...\n");
                                      //各種要素のリセット

                                     commandpara[C_Selecter.getWhichCharacters()].erase(commandpara[C_Selecter.getWhichCharacters()].begin() + C_Selecter.getCommandPos());
                                     atkpara[C_Selecter.getWhichCharacters()].erase(atkpara[C_Selecter.getWhichCharacters()].begin() + C_Selecter.getCommandPos());

                                     elementpara[C_Selecter.getWhichCharacters()].erase(elementpara[C_Selecter.getWhichCharacters()].begin() + C_Selecter.getCommandPos());
                                     skillpara[C_Selecter.getWhichCharacters()].erase(skillpara[C_Selecter.getWhichCharacters()].begin() + C_Selecter.getCommandPos());
                                     //下記を移動するとえらる
                                     //   usemppoint[C_Selecter.getWhichCharacters()].erase(usemppoint[C_Selecter.getWhichCharacters()].begin() + C_Selecter.getCommandPos());




                                     for (int i = 0; i < 256; i++)
                                     {
                                         if (Itemnum[i][1] == 0) // 個数が0なら詰める
                                         {
                                             for (int j = i; j < 255; j++)
                                             {
                                                 Itemnum[j][0] = Itemnum[j + 1][0];
                                                 Itemnum[j][1] = Itemnum[j + 1][1];
                                                 //printfDx("番号:%d\n", Itemnum[i][0]);
                                             }


                                         }
                                     }
                                 }
                                 else
                                 {
                                     //  printfDx("個数を減らします\n");

                                 }




                                 C_Selecter.setWhichCharacters(commandpara.size() - 1);//リセット
                                 C_Selecter.setSelecter(1);//敵を選ぶフラグを真にする.

                             }
                             //とくぎを選択し、MPがあるなら
                             else if (c.getMp() - usemppoint[C_Selecter.getOrderNumber()][C_Selecter.getCommandPos()] >= 0)
                             {
                                 // printfDx("取得したスキルの属性%d\n", elementpara[C_Selecter.getOrderNumber()][C_Selecter.getCommandPos()]);
                                  //選択したコマンドの攻撃力、属性を取得
                                 Manager.setAtk(c.getAtk(), atkpara[C_Selecter.getOrderNumber()][C_Selecter.getCommandPos()], elementpara[C_Selecter.getOrderNumber()][C_Selecter.getCommandPos()], skillpara[c.getOrder()][C_Selecter.getCommandPos()]);//攻撃力、skillの威力、技属性

                                 c.setMp(c.getMp() - usemppoint[C_Selecter.getOrderNumber()][C_Selecter.getCommandPos()]);
                                 //これより上


                                 loghistory.insert(loghistory.begin(), player.GetName() + "の" + commandpara[C_Selecter.getOrderNumber()][C_Selecter.getCommandPos()].c_str() + "!");

                                 C_Selecter.setWhichCharacters(commandpara.size() - 1);//リセット
                                 C_Selecter.setSelecter(1);//敵を選ぶフラグを真にする.

                             }
                             else
                             {
                                 loghistory.insert(loghistory.begin(), "しかし、" + player.GetName() + "はMPが足りない...");
                                 break;
                             }



                             break;
                         case ACTION_SKILL://とくぎ(特技のフォルダに移行)

                             C_Selecter.setWhichCharacters(c.getOrder());
                             //printfDx("コマンドに移行");



                             C_Selecter.setOrderNumber(c.getOrder());

                             C_Selecter.setEnyrAction(2);
                             break;

                         case ACTION_TOOLS://どうぐ(道具のフォルダに移行)

                             //道具を所持していたら
                             if (Itemnum[0][0] > 0)
                             {
                                 C_Selecter.setWhichCharacters(commandpara.size() - 2);//リセット
                                 // printfDx("道具を選びましょう\n");

                                 C_Selecter.setOrderNumber(commandpara.size() - 2);
                                 C_Selecter.setEnyrAction(3);
                             }
                             else
                             {
                                 loghistory.insert(loghistory.begin(), player.GetName() + "は道具を持っていない...");
                             }
                             break;

                         case ACITON_DEFENSE://ぼうぎょ

                             c.setgetDefenseFlg(TRUE);
                             c.setDef(c.getDef() * 1.5f);

                             loghistory.insert(loghistory.begin(), player.GetName() + "は守りを固めた!");

                             Manager.addCurrentTurn();//攻撃したのでターンを回す
                             //  printfDx("防御\n");
                             C_Selecter.setEnyrAction(4);
                             break;

                         case ACITON_ESCAPE://にげる
                             
                             //逃げの抽選
                             int num = GetRand(10);
                             
                             if (num <= 2)
                             {
                                 loghistory.insert(loghistory.begin(), "逃げられなかった...");

                             }
                             else 
                             {
                                 endGame(0);
                             }

                             Manager.addCurrentTurn();//攻撃したのでターンを回す


                             C_Selecter.setEnyrAction(5);
                             break;
                         }


                         //カーソルのリセット.
                         C_Selecter.UpdateCursor(0, C_Selecter.getArraySize(commandpara, c.getOrder()));
                     }
                     C_Selecter.DrawCommand(commandpara, usemppoint, Itemnum, C_Selecter.getWhichCharacters(), C_Selecter.getArraySize(commandpara, C_Selecter.getWhichCharacters()));//コマンドの描画(配列を送る)
                 }


                 //エネミーならランダム抽選
                 else if (c.getHp() > 0 && Manager.getEnemyFlg() == false)
                 {


                     //攻撃、回復スキルを割り振った
                     int AtackSkill[4] = { -1 };//アドレスを保存
                     int RecoverySkill[4] = { -1 };
                     int Acnt = 0;
                     int Rcnt = 0;

                     for (int i = 0; i < 4; i++)
                     {
                         if (atkpara[c.getOrder()][i] > 0)
                         {
                             int j = 0;
                             while (AtackSkill[j] > 0 && j < 3)
                             {
                                 j++;
                             }

                             AtackSkill[j] = i;
                             Acnt++;
                         }
                         else if (atkpara[c.getOrder()][i] < 0)
                         {
                             int j = 0;
                             while (RecoverySkill[j] > 0 && j < 3)
                             {
                                 j++;
                             }

                             RecoverySkill[j] = i;
                             Rcnt++;

                         }
                         else
                         {

                         }
                     }
                     //  printfDx("攻撃:%d, 回復:%d\n", Acnt, Rcnt);


                       //体力が満タンのやつ除外


                       //味方の体力が少ないと回復しやすくする
                     for (int i = 0; i <= C_Selecter.getSizeReserve(1); i++)
                     {
                         if (Charas[C_Selecter.getCharaList(1, i)].getHp() <= Charas[C_Selecter.getCharaList(1, i)].getMaxHp() / 2 && Charas[C_Selecter.getCharaList(1, i)].getHp() > 0)
                         {
                             //  printfDx("体力が半分以下の見方がいます(最大:%d, 現在:%d)", Charas[C_Selecter.getCharaList(1, i)].getMaxHp(), Charas[C_Selecter.getCharaList(1, i)].getHp());
                             c.setEnemyFeeling(-30);
                         }
                     }

                     int Rand = GetRand(99);

                     //攻撃したいか回復したいか




                     //c.getEnemyFeeling()は最大、最小値が設けられている

                     //強気であればあるほど可能性は高い(回復技を持っていなければ、攻撃で確定)
                     if (Rand <= c.getEnemyFeeling() || Rcnt == 0)
                     {

                         //3:7で攻撃:とくぎ(特技を持っていないなら攻撃)
                         Rand = GetRand(10);
                         if (Rand <= 3 || Acnt == 0)
                         {
                             //通常攻撃
                              //ログ
                             loghistory.insert(loghistory.begin(), enemy[numbers[c.getOrder()][0]].GetName() + "のこうげき！");
                             Manager.setEnemyFlg(true);
                             Manager.setAtk(c.getAtk(), 0, 0, 0);//攻撃力、skillの威力、技属性
                         }
                         else
                         {
                             int i = 0;
                             while (AtackSkill[i] == 0 && i < Acnt)
                             {
                                 i++;
                             }

                             loghistory.insert(loghistory.begin(), enemy[numbers[c.getOrder()][0]].GetName() + "の" + commandpara[c.getOrder()][AtackSkill[i]].c_str() + "！");
                             Manager.setEnemyFlg(true);
                             Manager.setAtk(c.getAtk(), atkpara[c.getOrder()][AtackSkill[i]], elementpara[c.getOrder()][AtackSkill[i]], skillpara[c.getOrder()][AtackSkill[i]]);//攻撃力、skillの威力、技属性

                         }
                         c.setEnemyFeeling(15);
                     }
                     else //回復
                     {
                        
                     int i = GetRand(Rcnt);
                     while (RecoverySkill[i] == 0)
                     {
                         i = GetRand(Rcnt);
                     }
                     
                     loghistory.insert(loghistory.begin(), enemy[numbers[c.getOrder()][0]].GetName() + "の" + commandpara[c.getOrder()][RecoverySkill[i]].c_str() + "！");
                     Manager.setEnemyFlg(true);
                     Manager.setAtk(c.getAtk(), atkpara[c.getOrder()][RecoverySkill[i]], elementpara[c.getOrder()][RecoverySkill[i]], skillpara[c.getOrder()][RecoverySkill[i]]);//攻撃力、skillの威力、技属性

                     c.setEnemyFeeling(35);

                 }



                     /*
                     int Rand = GetRand(99);

                     int cnt = 0;
                     if (Rand < 30)//通常攻撃
                     {
                         //ログ
                         loghistory.insert(loghistory.begin(), enemy[numbers[c.getOrder()][0]].GetName() + "のこうげき！");
                         Manager.setEnemyFlg(true);
                         Manager.setAtk(c.getAtk(), 0, 0, 0);//攻撃力、skillの威力、技属性
                     }
                     else//スキルで攻撃、ないなら通常攻撃
                     {
                         

                         Rand = GetRand(3);
                         while ((commandpara[c.getOrder()][Rand] == "なし") && cnt < 100)
                         {
                             cnt++;
                             Rand = GetRand(3);
                         }
                         if (cnt < 50)//スキルを用いた攻撃
                         {
                             loghistory.insert(loghistory.begin(), enemy[numbers[c.getOrder()][0]].GetName() + "の" + commandpara[c.getOrder()][Rand].c_str() + "！");

                             Manager.setEnemyFlg(true);
                             Manager.setAtk(c.getAtk(), atkpara[c.getOrder()][Rand], elementpara[c.getOrder()][Rand], skillpara[c.getOrder()][Rand]);//攻撃力、skillの威力、技属性

                         }
                         else//敵が技を持っていない場合も、やっぱり通常攻撃
                         {
                             loghistory.insert(loghistory.begin(), enemy[numbers[c.getOrder()][0]].GetName() + "のこうげき！");
                             Manager.setEnemyFlg(true);
                             Manager.setAtk(c.getAtk(), 0, 0, 0);//攻撃力、skillの威力、技属性
                         }


                         
                     }*/

                 }
             }
         }
     }




    /*ゲーム終了時に終わらせたい.
    // 読み込んだ画像メモリの開放
    chara::UnloadImage();
    */

}




