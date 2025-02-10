#ifndef __SENTOU_H__
#define __SENTOU_H__

#include "Data.h"
#include <stdexcept>




// charaクラスの定義
class Chara {
private:
    int x, y;
    int number;
    int hp, mp, atk, spd, def, level, element;
    int maxhp, maxmp;//hpバーの描画のために使用
    int order;
    bool defenseflg = false;//防御中かどうか
    bool deathflg = false;//死んでしまったかどうか


    //下記変巣は敵のみ使用(classを敵味方で分けるとき注意)
    int enemyfeeling = 100;//敵が強気か弱気か(0が弱気、100が強気) (30 ～ 100の範囲)



    static const int divisionsx = 1;  // 横の分割数
    static const int divisionsy = 1;  // 縦の分割数
    static const int totalImages = divisionsx * divisionsy; // 総画像数

    static int image[totalImages];  // 分割された画像を保持する配列

public:
    //下記変数のみpublicなのは、外部からCharaクラス共通のsize変数を取得でっ切るようにするため.
    //ゲッターを用意してもいいが、そうするとインスタンスされたCharaクラスのオブジェクトを経由しないといけなくて不便.
    static int size;//画像サイズ.

    // コンストラクタ
    Chara(int startX, int startY, int charaNumber, int charahp, int charamp, int charaatk, int charadef, int charaspd, int charalevel, int charaelement, int charaorder)
        : x(startX), y(startY), number(charaNumber), hp(charahp), mp(charamp), atk(charaatk), def(charadef), spd(charaspd), level(charalevel), element(charaelement), order(charaorder), maxhp(charahp), maxmp(charamp) {}

    // デストラクタ 
    ~Chara() {}


    //ゲッター.
    int getX() const { return x; }
    int getY() const { return y; }
    int getNumber() const { return number; }
    int getSize() const { return size; }
    int getHp() const { return hp; }
    int getMaxHp() const { return maxhp; }
    int getMp() const { return mp; }
    int getMaxMp() const { return maxmp; }
    int getAtk() const { return atk; }
    int getSpd() const { return spd; }
    int getDef() const { return def; }
    int getElement() const { return element; }
    int getLevel() const { return level; }
    int getOrder() const { return order; }
    bool getDefenseFlg()const { return defenseflg; }
    bool getDeathFlg()const { return deathflg; }

    int getEnemyFeeling()const { return enemyfeeling; }



    //セッター.
    void setX(int newX) { x = newX; }
    void setY(int newY) { y = newY; }
    void setNumber(int newnumber) { number = newnumber; }
    void setHp(int newhp) { hp = newhp; if (hp < 0)hp = 0; if (hp > maxhp)hp = maxhp; }
    void setMp(int newmp) { mp = newmp; if (mp < 0)mp = 0; if (mp > maxmp)mp = maxmp; }
    void setAtk(int newatk) { atk = newatk; }
    void setSpd(int newspd) { spd = newspd; }
    void setDef(int newdef) { def = newdef; }
    void setLevel(int newlevel) { level = newlevel; }
    void setgetDefenseFlg(int nerflg) { defenseflg = nerflg; }
    void setDeathFlg(bool newflg) { deathflg = newflg; }

    void setEnemyFeeling(int newint) { enemyfeeling += newint; if (enemyfeeling <= 30)enemyfeeling = 30; if (enemyfeeling >= 100)enemyfeeling = 100; }

    // 描画関数
    void Draw() {
        if (image[number] != -1) {

            DrawExtendGraph(x, y, x + size, y + size, image[0], TRUE);
        }
    }





    void DrawData(int PorE)//0が敵、1がプレイヤー
    {
        unsigned int Color[5] = {
              GetColor(255, 255, 255), //枠
              GetColor(0,0,0),//黒
              GetColor(0,155,0),//HPバー緑
              GetColor(255,0,0),//Hpバー赤
              GetColor(0,0,255),//MPバー青

        };

        int xOffset[4] = { 20, 80, 40, 60 };//40,60がバーのサイズ
        int yOffset[4] = { 5, 15 , 35, 45 };//値の微調整に使用

        if (PorE == 1)
        {
            /***********************HPバーの表示****************************/
            DrawBox(x + xOffset[2], y + size + yOffset[0], x + xOffset[2] + xOffset[3], y + size + yOffset[1], Color[3], true);//Hpバー赤
            if (hp > 0)
            {
                DrawBox(x + xOffset[2], y + size + yOffset[0], x + xOffset[2] + (xOffset[3] * (static_cast<float>(hp) / maxhp)), y + size + yOffset[1], Color[2], true);//Hpバー緑
            }
            DrawString(x + xOffset[0], y + size, "HP", Color[0]);
            //std::tostringでintがた変数をstring型に変更、c_strで*char型に変更.
            DrawString(x + xOffset[1], y + size - 5, std::to_string(hp).c_str(), Color[0]);
            /***********************HPバーの表示****************************/

            /***********************MPバーの表示****************************/
            DrawBox(x + xOffset[2], y + size + yOffset[2], x + xOffset[2] + xOffset[3], y + size + yOffset[3], Color[3], true);//Hpバー赤
            if (mp > 0)
            {
                DrawBox(x + xOffset[2], y + size + yOffset[2], x + xOffset[2] + (xOffset[3] * (static_cast<float>(mp) / maxmp)), y + size + yOffset[3], Color[4], true);//Hpバー緑
            }
            DrawString(x + xOffset[0], y + size + 30, "MP", Color[0]);

            DrawString(x + xOffset[1], y + size + 25, std::to_string(mp).c_str(), Color[0]);
            /***********************MPバーの表示****************************/

            //一応レベル
            DrawString(x + xOffset[1], y + size - 25, std::to_string(level).c_str(), Color[0]);
            DrawString(x + xOffset[1] - 30, y + size - 25, "Lv:", Color[0]);
        }
        else if (PorE == 0)
        {

            /***********************HPバーの表示****************************/
            DrawBox(x + xOffset[2], y + size + yOffset[0], x + xOffset[2] + xOffset[3], y + size + yOffset[1], Color[3], true);//Hpバー赤
            if (hp > 0)
            {
                DrawBox(x + xOffset[2], y + size + yOffset[0], x + xOffset[2] + (xOffset[3] * (static_cast<float>(hp) / maxhp)), y + size + yOffset[1], Color[2], true);//Hpバー緑
            }


            DrawString(x + xOffset[0], y + size, "HP", Color[0]);


            //std::tostringでintがた変数をstring型に変更、c_strで*char型に変更.
            //DrawString(x + xOffset[1], y + size - 5, std::to_string(hp).c_str(), Color[0]);
            /***********************HPバーの表示****************************/
        }
    }


    // 画像読み込み関数
    static void LoadImage(const char* ImagePath) {
        if (LoadDivGraph(ImagePath, totalImages, divisionsx, divisionsy, 64, 64, image) == -1) {//64は分割サイズ.
            printfDx("画像の読み込みに失敗しました: %s\n", ImagePath);
        }
    }

    // 画像解放関数
    static void UnloadImage() {
        for (int i = 0; i < totalImages; ++i) {
            if (image[i] != -1) {
                DeleteGraph(image[i]);
                image[i] = -1;
            }
        }
    }




};

class MatchManager
{
private:
    bool anyflg = false;//なんかフラグを使いたいなと思ったらこれ.
    bool fastrogflg = false;
    int anynumber = -1;//なんか番号を使いたいときに.
    bool gameflg = true;//ゲームが進行中かどうか.
    int winnerflg = 0;//勝ち負けのフラグ1が勝ち、-1が負け
    bool lognumberflg[10] = { 0 };//勝ち負け時に表示させるログを段階的に表示させるためのやつ.
    int datacnt = 0;//全部の敵、味方の数.
    int playercnt = 0;//味方の数
    int enemycnt = 0;//敵の数
    int currentturn = 0;//現在誰のターンかのカウント.
    int turn = 0;//経過したターン数等のカウント
    bool enemyflg = false;//敵のターンを行う
    int atk = 0;//攻撃する側の攻撃力
    int skillatk = 0;//とくぎの攻撃力
    bool skillflg = 0;//全体攻撃か否か、真なら全体
    int def = 0;//攻撃される側の防御力
    int atkelement = 0;//攻撃側属性
    int defelement = 0;//防御側属性
    static float element[16][10]; //相性表に使用 [攻撃側][防御側](個々の配列の数はキャラ数の変数にしたいかも)
    int time = 0;//現在の待機時間
    int randomnumber = 4;//乱数の最大値.
    static const int maxdata = 20;//staticでクラスに一つだけに、constで値が変化しない証明(画面に存在できるキャラ数の制限)
    int order[maxdata][2];//spd,number
    int weaponid = 0; //敵がドロップした装備のid




    int itemtype[2] = { 0 };
    int allexp = 0;
    int allgold = 0;

    bool levelupflg = false;

    bool fastsetflg = true;

    static int image;
    static int background;

public:
    // コンストラクタで配列をすべて -1 で初期化
    MatchManager()
    {
        for (int i = 0; i < maxdata; ++i) {
            order[i][0] = -1;
            order[i][1] = -1;
        }
    }
    //デストラクタ.
    ~MatchManager() {}

    //セッター.
    void setAnyFlg(bool newflg) { anyflg = newflg; }
    void setAnyNumber(int newnum) { anynumber = newnum; }

    void addAnyNumber() { anynumber++; }
    void setDataCnt(int newDataCnt) { datacnt = newDataCnt; }
    void addPlayerCnt() { playercnt++; }
    void addEnemyCnt() { enemycnt++; }
    void addTurn() { turn++; }//ターンは+1ずつしかしないのでそれ用も.
    void setTurn(int newTurn) { turn = newTurn; }
    void addCurrentTurn() { currentturn++; }//ターンの中で誰が行動したか管理する.
    void setCurrentTurn(int newcurrentturn) { currentturn = newcurrentturn; }
    void setAtk(int newatk, int newskillatk, int newelement, int newskillflg) { atk = newatk; skillatk = newskillatk;  atkelement = newelement; skillflg = newskillflg; }//攻撃側の各種データを取得
    void setDef(int newdef, int newelement) { def = newdef; defelement = newelement; }//防御力、属性の取得
    void setEnemyFlg(bool newflg) { enemyflg = newflg; }
    void setSkillFlg(bool newflg) { skillflg = newflg; }
    void setLogNumberFlg(int number, bool newflg) { lognumberflg[number] = newflg; }
    void addAllExp(int exp) { allexp += exp; }
    void addAllGold(int gold) { allgold += gold; }
    void setItemType(int hp, int mp) { itemtype[0] = hp; itemtype[1] = mp; /*printfDx("HP:%d,MP:%d\n", hp, mp); */ }
    void setGameFlg(bool newflg) { gameflg = newflg; }
    void setWinnerflg(int newnumber) { winnerflg = newnumber; }
    void setWeaponId(int newweapon) { weaponid = newweapon; }
    void setLevelUpFlg(bool newflg) { levelupflg = newflg; }

    //ゲッター(constをつけて値を変更しないことの証明).
    bool getAnyFlg() const { return anyflg; }
    int getAnyNumber() const { return anynumber; }
    bool getGameFlg() const { return gameflg; }
    int getWinnerFlg() const { return winnerflg; }
    bool getLogNumberFlg(int number) const { return lognumberflg[number]; }
    int getData() const { return datacnt; }
    int getPlayerCnt() const { return playercnt; }
    int getEnemyCnt() const { return enemycnt; }
    int getTurn() const { return turn; }
    int getOrder(int receiveOrder)const { return order[receiveOrder][1]; }//Orderの値だけ参照.
    int getCurrentTurn() const { return currentturn; }
    int getSkillAtk() const { return skillatk; }
    bool getEnemyFlg() const { return enemyflg; }
    int getSkillFlg() const { return skillflg; }
    int getSkillatk() const { return skillatk; }
    int getAllExp() const { return allexp; }
    int getAllGold() const { return allgold; }
    int getItemType(int number) const { return itemtype[number]; }
    int getWeaponId()const { return weaponid; }
    bool getLevelUpFlg()const { return levelupflg; }

    int Atack(int action)  //攻撃の威力の計算、送信
    {

        int dmg = 0;
        //atk+スキル攻撃力+(+-5の乱数)*相性倍率-防御力
        if (skillatk >= 0)//攻撃スキルなら
        {
            //攻撃力　＋スキル威力　＋(乱数　ー防御力　)*相性
            dmg = ((atk + skillatk + (GetRand(randomnumber) - randomnumber / 2) - def)) * element[defelement][atkelement];//防御側の攻撃側に対する値

            if (dmg <= 0)
            {
                dmg = 1;
            }


        }

        else if (action == 2 || skillatk < 0)//回復スキルなら
        {

            dmg = skillatk - (GetRand(randomnumber));

            if (dmg >= 0)
            {
                dmg = -1;
            }
        }
        else if (action == 3)//回復道具なら
        {

            dmg = skillatk;

        }


        return dmg;
    }
    bool Stay(int staytime)//  一定時間待機する
    {
        //return true;
        if (staytime > time)
        {
            time++;
            return false;
        }
        else
        {
            time = 0;
            return true;
        }

    }
    bool onlyStay(int staytime)//  一定時間待機する(一回きり)
    {

        if (staytime > time)
        {
            time++;
            return false;
        }
        else
        {
            return true;
        }

    }



    void risetAtackData()
    {
        atk = 0;
        skillatk = 0;
        def = 0;
        atkelement = 0;
        defelement = 0;

    }


    // 配列が空ならデータの保存 0:素早さ、1,割り振られた番号
    void setSpdAndOrder(int setspd, int setorder)
    {
        if (order[datacnt][0] == -1 && order[datacnt][1] == -1)
        {
            order[datacnt][0] = setspd;
            order[datacnt][1] = setorder;
            datacnt += 1;
        }
    }


    //取得したデータを素早さの順番に並べ替える
    void orderSelect()
    {
        //何回もループしろ
        for (int i = 0; i < datacnt;)//datacntはさっきキャラ数をカウントしていた.
        {
            //もし1より2の方が速いのなら
            if (order[i][0] < order[i + 1][0])
            {
                //箱を用意して入れ替え.
                int changespd, changorder;
                changespd = order[i][0];
                changorder = order[i][1];

                order[i][0] = order[i + 1][0];
                order[i][1] = order[i + 1][1];

                order[i + 1][0] = changespd;
                order[i + 1][1] = changorder;

                i = 0;//入れ替えを行ったら最初から再確認.
            }
            else i++;
        }
    }




    //行動が一周したか調べる.
    bool ChackCurrentTurn()
    {
        return(currentturn > datacnt);
    }

    //コマンドでも表示させたい＋黄色の枠の変更.
    void DrawFrame(int x1, int y)
    {
        int addwindowsize = 30;//大きさの調整
        unsigned int Color[4] = {
           GetColor(255, 255, 255), //枠
           GetColor(0,0,0),//黒
           GetColor(0,255,0),//HPバー緑
           GetColor(255,0,0),//Hpバー赤

        };

        DrawBox(x1, y, x1 + (playercnt * Chara::size), y + Chara::size * 2, Color[1], true);//枠内
        DrawBox(x1, y, x1 + (playercnt * Chara::size), y + Chara::size * 2, Color[0], false);//枠


        DrawExtendGraph(x1 - addwindowsize, y - addwindowsize,
            x1 + addwindowsize + (playercnt * Chara::size), y + addwindowsize + Chara::size * 2, image, TRUE);
    }

    void DrawOnlyFrame(int x1, int y1, int x2, int y2)
    {
        DrawExtendGraph(x1, y1, x2, y2, image, TRUE);

    }

    void DrawBackGround(int x1, int y1, int x2, int y2)
    {
        DrawExtendGraph(x1, y1, x2, y2, background, TRUE);

    }

    // 配列の内容を表示する関数（デバッグ用）
    void showData()
    {
        // 表示する位置の初期座標 (x, y) を設定
        int posX = 1000;  // X座標
        int posY = 10;   // Y座標 (初期値)

        for (int i = 0; i < datacnt; ++i)
        {
            // turn[i] の2つの値（素早さと順番）をそれぞれ表示
            char buffer[100];
            snprintf(buffer, sizeof(buffer), "turn[%d]: SPD = %d, Order = %d", i, order[i][0], order[i][1]);

            // 画面に文字列を描画 (x, y座標, 色)
            DrawString(posX, posY, buffer, GetColor(255, 255, 255));

            // 描画位置を次の行に移動
            posY += 20;  // 20ピクセル下に移動
        }

        DrawString(0, 600, std::to_string(getCurrentTurn()).c_str(), 0x00ff00);//1ターンの中で何人行動か.
        DrawString(0, 630, std::to_string(getData()).c_str(), 0x00ff00);//1ターンの中で何番目の行動か.
    }

    void risetAllData()//全データの削除.
    {
        datacnt = 0;//全部の敵、味方の数.
        playercnt = 0;
        enemycnt = 0;
        currentturn = 0;//現在誰のターン化のカウント.
        turn = 0;//経過したターン数等のカウント

        for (int i = 0; i < maxdata; ++i) {
            order[i][0] = -1;
            order[i][1] = -1;
        }
    }


    static void LoadImage(const char* ImagePath)
    {
        image = LoadGraph(ImagePath);
        if (image == -1)
        {
            printfDx("画像の読み込みに失敗しました: %s\n", ImagePath);
        }
    }


    static void LoadImageBackground(const char* ImagePath)
    {
        background = LoadGraph(ImagePath);

    }



    static void unloadImage()
    {
        if (image != -1)
        {
            DeleteGraph(image);
            image = -1;
        }
    }

    //下記二つはあまりきれいじゃない気がする。一回きりの動作にはこだわりたいね
    bool fastSet()
    {
        if (fastsetflg == true)
        {
            fastsetflg = false;
            return true;
        }
        return false;
    }
    void risetFastSet(bool newflg) { fastsetflg = newflg; }


};


class  CommandSelector
{

private:

    int x = 1000;
    int y = 400;
    static const int sizex = 256;
    static const int sizey = 256;
    static const int limitlist = 10;//保存できる、敵、プレイヤーの最大数(マッチmanagerにあるmaxdataとかぶる)
    std::vector<std::vector<int>> charalist;

    //  int charalist[2][limitlist] = { 0 };//すべて初期化しろ!!!
    bool selecter = false;//敵を選択するターンか
    //  static const int staytime = 10;//選択カーソルの待機時間

    int cursorpos = 0;//現在のセレクターの場所(敵の選択用).
    int commandpos = 0;//技選択のカーソル(コマンド選択のために複数個所で使用)
    int commandcursor = 0;//技選択の描画用に使用
    int whichcharacters = 0;//誰のコマンドを描画するか.
    int range = 3;//コマンドを何個描画するか0~3(値が変わると対応できない)
    int ordernumber = 0;//コマンドの情報を取得するときの住所を保存
    int enyraction = 0;//行動がどれに当たるか(0,攻撃1,特技2,どうぐ3,防御4,にげる)
    int addres = 0;//コマンドの描画

    const char* action[20] = { "こうげき", "とくぎ", "どうぐ", "にげる", "5", "6", "7", "8", "9", "10", "11", "12", "13" };




    //攻撃力、skillの威力、技属性

public:

    //コンストラクタ
    CommandSelector() {
        charalist.resize(2);

    }
    //デストラクタ
    ~CommandSelector() {}


    //ゲッター.
    int getX() const { return x; }
    int getY() const { return y; }
    //int getPlayerList(int number) const{ return playerlist[number]; }
   // int getEnemyList(int number) const { return enemylist[number]; }
    int getCharaList(int playerorenemy, int number) const { return charalist[playerorenemy][number]; }

    bool getSelecter() const { return selecter; }
    int  getCursorPos() const { return  cursorpos; }
    int getCommandPos()const { return  commandpos; }
    int getLimitList() const { return limitlist; }

    int getWhichCharacters()const { return whichcharacters; }
    int getOrderNumber()const { return ordernumber; }

    int getEnyrAction()const { return enyraction; }
    void setEnyrAction(int num) { enyraction = num; }

    //セッター.
    void setX(int newx) { x = newx; }
    void setY(int newy) { y = newy; }
    void setSelecter(bool newselecter) { selecter = newselecter; }
    void setCursorPos(int newcursorpos) { cursorpos = newcursorpos; }

    void setWhichCharacters(int newCharacters) { whichcharacters = newCharacters; };

    void setOrderNumber(int newnumber) { ordernumber = newnumber; }




    void setCharaList(int Pnumber, int Enumber)
    {
        // プレイヤー番号をcharalist[0]に追加
        if (Pnumber != -1) {
            charalist[0].push_back(Pnumber);
        }

        // 敵番号をcharalist[1]に追加
        if (Enumber != -1) {
            charalist[1].push_back(Enumber);
        }
    }




    bool chackEnemyList(int PorE, int number) {//右に進んでいいか

        return (charalist[PorE].size() > number + 1);

    }

    int getSizeReserve(int num)//エネミーキャラが現在何体いるか調べる0が味方、1が敵
    {
        return charalist[num].size() - 1;
    }



    //commandparaのサイズを確認する
    int getArraySize(const std::vector<std::vector<std::string>>& array, int number) {
        if (number >= 0 && number < array.size()) {//ちゃんと存在する範囲を調べるか検証.

            return array[number].size();
        }
        else {
            return 0;
        }
    }



    //コマンド選択が範囲外のアドレスに行っちゃう！
    void UpdateCursor(int direction, int arraysize)
    {
        // -1または1以外の値が来たらカーソルの設定をリセット
        if (direction != -1 && direction != 1)
        {
            commandpos = 0;
            addres = 0;
            commandcursor = 0;
            return;
        }

        // 配列サイズが 0 の場合は処理をしない
        if (arraysize <= 0) return;

        // commandcursor を更新
        commandcursor += direction;

        // arraysize が 4 未満の場合は制限を厳しくする
        int cursorLimit;
        if (arraysize < 4)
        {
            cursorLimit = arraysize - 1;
        }
        else
        {
            //4以上なら従来通り
            cursorLimit = 3;
        }


        // commandcursor が 0 未満の場合、addres を減らす
        if (commandcursor < 0)
        {
            if (addres > 0)
            {
                addres--;
                commandcursor = 0;  // commandcursor は 0 未満にならないように調整
            }
            else
            {
                commandcursor = 0;
            }
        }

        // commandcursor が制限値 (cursorLimit) を超えた場合、addres を増やす
        if (commandcursor > cursorLimit)
        {
            if (arraysize >= 4 && addres < arraysize - 4) // 配列サイズが 4 以上のときのみスクロール
            {
                addres++;
                commandcursor = cursorLimit;
            }
            else
            {
                commandcursor = cursorLimit;
            }
        }

        // 最終的な commandpos の計算（commandcursor + addres）
        commandpos = commandcursor + addres;

        // 配列サイズを超えないように制限
        if (commandpos >= arraysize)
        {
            commandpos = arraysize - 1;
        }
    }


    void DrawCommand(const std::vector<std::vector<std::string>>& commandpara, const std::vector<std::vector<int>>& usemppoint, int(&Itemnum)[256][2], int number, int size)
    {
        /*
        DrawString(0, 130, (std::string("スタートポス:") + std::to_string(commandpos)).c_str(), 0xff0fff);
        DrawString(0, 150, (std::string("カーソル;") + std::to_string(commandcursor)).c_str(), 0xff0fff);
        DrawString(0, 170, (std::string("アドレス:") + std::to_string(addres)).c_str(), 0xff0fff);
        */
        int x = 950, y = 450;
        int x2 = 200;//コマンドのとなりに表示するMPのPos
        int pos = 0;//commandの描画用

        //所持コマンドが表示できる数以上なら
        if (size > range)
        {
            size = range + 1;
        }

        //commandの表示
        for (int i = 0; i < size; i++)
        {
            DrawString(x, y + pos * 20, commandpara[number][i + addres].c_str(), 0xffffff);

            //とくぎ一覧を表示の時時
            if (number != commandpara.size() - 1 && number != commandpara.size() - 2)
            {
                DrawString(x + x2, y + pos * 20, ("MP" + std::to_string(usemppoint[number][i + addres])).c_str(), 0xffffff);
            }
            else if (number != commandpara.size() - 1)
            {

                //    DrawFormatString(x + x2, y + pos * 20, 0xffffff, "%d個",  player.GetPosItem(i, 1));

                DrawFormatString(x + x2, y + pos * 20, 0xffffff, "%d個", Itemnum[i][1]);

            }
            pos++;
        }

        DrawBox(x, y + commandcursor * 20, x + 200, y + (commandcursor + 1) * 20, 0xfff000, false);
    }


    void kakuninDraw(int num)
    {
        //if(num >= 0)
        //  DrawString(0, 330, action[commandpos], 0xff0fff);



    }

    bool CheckDataisNumber()
    {
        //数字なら
        return true;

        //文字なら

    }


    bool checkBattleEnds(int target, std::vector<Chara>& Charas)
    {
        int num = 0;

        while (Charas[getCharaList(target, num)].getHp() == 0)
        {
            num++;

            if (charalist[target].size() <= num)
            {
                return true;
            }

        }
        return false;
    }



    /*メモです。配列の要素が必要な時、引数に使用する型はどんなものがいいか
                  メリット	                   デメリット
    値渡し	   元の配列に影響を与えない。 	配列のコピーが発生。
     const &   コピーが不要で効率的。    	元の配列への参照が必要。
    */
    void checkCanShift(int target, int direction, std::vector<Chara>& Charas)
    {

        //Mでリセットすると右に行けすぎる
        int num = 1;
        // 生きているキャラの検索
        switch (direction)
        {
        case 1://D

            //生きているキャラの検索.
            while (Charas[getCharaList(target, getCursorPos() + num)].getHp() == 0 &&
                chackEnemyList(target, getCursorPos() + num))
            {
                num++;
            }

            //numが範囲外にならないようにチェック.
            while (Charas[getCharaList(target, getCursorPos() + num)].getHp() == 0)
            {
                num--;
            }

            //検索したアドレスに移動.
            setCursorPos(getCursorPos() + num);

            break;

        case -1://A
            //生きているキャラの検索.
            while (Charas[getCharaList(target, getCursorPos() - num)].getHp() == 0 &&
                getCursorPos() - num > 0)
            {
                num++;
            }
            while (Charas[getCharaList(target, getCursorPos() - num)].getHp() == 0)
            {
                num--;
            }
            //検索したアドレスに移動.
            setCursorPos(getCursorPos() - num);

            break;
        }
    }



    int const checkStringOrInt(int x, int y, std::vector<std::vector<std::string>> commandpara)//0は攻撃,1はとくぎ,2はどうぐ,3逃げる
    {
        /*
        1.攻撃
        2.特技の選択
        3.道具の選択
        4.防御
        5.逃げる
        */


        // printfDx("%s", commandpara[x][y].c_str());

        try {//エラーが出たら処理を中止する.
            std::stoi(commandpara[x][y]);  // 変換できる場合、整数
            return  0;//攻撃
        }
        catch (const std::invalid_argument&) {
            // 変換できない場合（文字列として扱う）

            if (commandpara[x][y] == "こうげき")
            {
                return 0;
            }
            if (commandpara[x][y] == "とくぎ")
            {
                return 2;
            }
            else if (commandpara[x][y] == "どうぐ")
            {
                return 3;
            }
            else if (commandpara[x][y] == "ぼうぎょ")
            {
                return 4;
            }
            else if (commandpara[x][y] == "にげる")
            {
                return 5;
            }
            else
            {
                return 1;//上記以外は特技名に当たる
            }
        }
    }
};

//ボタンの入力にのみ使用,
class InputManager {
private:
    //各キーのボタンが押された瞬間かどうか判別するために使用.
    static const int KEY_CODE_MAX = 256;
    bool hitFlags[KEY_CODE_MAX] = { false };  // 各キーの押下状態を保持する配列.

public:
    // コンストラクタ.
    InputManager() {}

    // デストラクタ.
    ~InputManager() {}

    // 指定したキーコードが押された瞬間だけtrueを返す.
    bool CheackAnyKeyDown(int keyCode) {
        // キーコードが範囲内であるか確認.
        if (keyCode < 0 || keyCode >= KEY_CODE_MAX) {
            return false;  // 範囲外の場合はfalse を返す.
        }

        // キーが押されていて、該当キーのフラグがfalseの場合.
        if (CheckHitKey(keyCode) && !hitFlags[keyCode]) {
            hitFlags[keyCode] = true;  // 押された瞬間にフラグを true にする.
            return true;               // 押された瞬間のみ true を返す.
        }
        else if (!CheckHitKey(keyCode)) {
            hitFlags[keyCode] = false;  // キーが離されたらフラグをリセット.
        }
        return false;  // 押しっぱなし、または押されていない場合は false.
    }
    // 指定したキーコードが押されている間trueを返す.
    bool CheackAnyKeyHold(int keyCode) {
        // キーコードが範囲内であるか確認.
        if (keyCode < 0 || keyCode >= KEY_CODE_MAX) {
            return false;  // 範囲外の場合はfalse を返す.
        }

        // キーが押されていて、該当キーのフラグがfalseの場合.
        if (CheckHitKey(keyCode)) {
            return true;
        }
        else return false;//押されていない場合は false.
    }




};


void sentou();
void allDelete();
void setEnemys(int);

#endif
