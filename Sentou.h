#ifndef __SENTOU_H__
#define __SENTOU_H__

#include "Data.h"
#include <stdexcept>




// chara�N���X�̒�`
class Chara {
private:
    int x, y;
    int number;
    int hp, mp, atk, spd, def, level, element;
    int maxhp, maxmp;//hp�o�[�̕`��̂��߂Ɏg�p
    int order;
    bool defenseflg = false;//�h�䒆���ǂ���
    bool deathflg = false;//����ł��܂������ǂ���


    //���L�ϑ��͓G�̂ݎg�p(class��G�����ŕ�����Ƃ�����)
    int enemyfeeling = 100;//�G�����C����C��(0����C�A100�����C) (30 �` 100�͈̔�)



    static const int divisionsx = 1;  // ���̕�����
    static const int divisionsy = 1;  // �c�̕�����
    static const int totalImages = divisionsx * divisionsy; // ���摜��

    static int image[totalImages];  // �������ꂽ�摜��ێ�����z��

public:
    //���L�ϐ��̂�public�Ȃ̂́A�O������Chara�N���X���ʂ�size�ϐ����擾�ł��؂�悤�ɂ��邽��.
    //�Q�b�^�[��p�ӂ��Ă��������A��������ƃC���X�^���X���ꂽChara�N���X�̃I�u�W�F�N�g���o�R���Ȃ��Ƃ����Ȃ��ĕs��.
    static int size;//�摜�T�C�Y.

    // �R���X�g���N�^
    Chara(int startX, int startY, int charaNumber, int charahp, int charamp, int charaatk, int charadef, int charaspd, int charalevel, int charaelement, int charaorder)
        : x(startX), y(startY), number(charaNumber), hp(charahp), mp(charamp), atk(charaatk), def(charadef), spd(charaspd), level(charalevel), element(charaelement), order(charaorder), maxhp(charahp), maxmp(charamp) {}

    // �f�X�g���N�^ 
    ~Chara() {}


    //�Q�b�^�[.
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



    //�Z�b�^�[.
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

    // �`��֐�
    void Draw() {
        if (image[number] != -1) {

            DrawExtendGraph(x, y, x + size, y + size, image[0], TRUE);
        }
    }





    void DrawData(int PorE)//0���G�A1���v���C���[
    {
        unsigned int Color[5] = {
              GetColor(255, 255, 255), //�g
              GetColor(0,0,0),//��
              GetColor(0,155,0),//HP�o�[��
              GetColor(255,0,0),//Hp�o�[��
              GetColor(0,0,255),//MP�o�[��

        };

        int xOffset[4] = { 20, 80, 40, 60 };//40,60���o�[�̃T�C�Y
        int yOffset[4] = { 5, 15 , 35, 45 };//�l�̔������Ɏg�p

        if (PorE == 1)
        {
            /***********************HP�o�[�̕\��****************************/
            DrawBox(x + xOffset[2], y + size + yOffset[0], x + xOffset[2] + xOffset[3], y + size + yOffset[1], Color[3], true);//Hp�o�[��
            if (hp > 0)
            {
                DrawBox(x + xOffset[2], y + size + yOffset[0], x + xOffset[2] + (xOffset[3] * (static_cast<float>(hp) / maxhp)), y + size + yOffset[1], Color[2], true);//Hp�o�[��
            }
            DrawString(x + xOffset[0], y + size, "HP", Color[0]);
            //std::tostring��int�����ϐ���string�^�ɕύX�Ac_str��*char�^�ɕύX.
            DrawString(x + xOffset[1], y + size - 5, std::to_string(hp).c_str(), Color[0]);
            /***********************HP�o�[�̕\��****************************/

            /***********************MP�o�[�̕\��****************************/
            DrawBox(x + xOffset[2], y + size + yOffset[2], x + xOffset[2] + xOffset[3], y + size + yOffset[3], Color[3], true);//Hp�o�[��
            if (mp > 0)
            {
                DrawBox(x + xOffset[2], y + size + yOffset[2], x + xOffset[2] + (xOffset[3] * (static_cast<float>(mp) / maxmp)), y + size + yOffset[3], Color[4], true);//Hp�o�[��
            }
            DrawString(x + xOffset[0], y + size + 30, "MP", Color[0]);

            DrawString(x + xOffset[1], y + size + 25, std::to_string(mp).c_str(), Color[0]);
            /***********************MP�o�[�̕\��****************************/

            //�ꉞ���x��
            DrawString(x + xOffset[1], y + size - 25, std::to_string(level).c_str(), Color[0]);
            DrawString(x + xOffset[1] - 30, y + size - 25, "Lv:", Color[0]);
        }
        else if (PorE == 0)
        {

            /***********************HP�o�[�̕\��****************************/
            DrawBox(x + xOffset[2], y + size + yOffset[0], x + xOffset[2] + xOffset[3], y + size + yOffset[1], Color[3], true);//Hp�o�[��
            if (hp > 0)
            {
                DrawBox(x + xOffset[2], y + size + yOffset[0], x + xOffset[2] + (xOffset[3] * (static_cast<float>(hp) / maxhp)), y + size + yOffset[1], Color[2], true);//Hp�o�[��
            }


            DrawString(x + xOffset[0], y + size, "HP", Color[0]);


            //std::tostring��int�����ϐ���string�^�ɕύX�Ac_str��*char�^�ɕύX.
            //DrawString(x + xOffset[1], y + size - 5, std::to_string(hp).c_str(), Color[0]);
            /***********************HP�o�[�̕\��****************************/
        }
    }


    // �摜�ǂݍ��݊֐�
    static void LoadImage(const char* ImagePath) {
        if (LoadDivGraph(ImagePath, totalImages, divisionsx, divisionsy, 64, 64, image) == -1) {//64�͕����T�C�Y.
            printfDx("�摜�̓ǂݍ��݂Ɏ��s���܂���: %s\n", ImagePath);
        }
    }

    // �摜����֐�
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
    bool anyflg = false;//�Ȃ񂩃t���O���g�������ȂƎv�����炱��.
    bool fastrogflg = false;
    int anynumber = -1;//�Ȃ񂩔ԍ����g�������Ƃ���.
    bool gameflg = true;//�Q�[�����i�s�����ǂ���.
    int winnerflg = 0;//���������̃t���O1�������A-1������
    bool lognumberflg[10] = { 0 };//�����������ɕ\�������郍�O��i�K�I�ɕ\�������邽�߂̂��.
    int datacnt = 0;//�S���̓G�A�����̐�.
    int playercnt = 0;//�����̐�
    int enemycnt = 0;//�G�̐�
    int currentturn = 0;//���ݒN�̃^�[�����̃J�E���g.
    int turn = 0;//�o�߂����^�[�������̃J�E���g
    bool enemyflg = false;//�G�̃^�[�����s��
    int atk = 0;//�U�����鑤�̍U����
    int skillatk = 0;//�Ƃ����̍U����
    bool skillflg = 0;//�S�̍U�����ۂ��A�^�Ȃ�S��
    int def = 0;//�U������鑤�̖h���
    int atkelement = 0;//�U��������
    int defelement = 0;//�h�䑤����
    static float element[16][10]; //�����\�Ɏg�p [�U����][�h�䑤](�X�̔z��̐��̓L�������̕ϐ��ɂ���������)
    int time = 0;//���݂̑ҋ@����
    int randomnumber = 4;//�����̍ő�l.
    static const int maxdata = 20;//static�ŃN���X�Ɉ�����ɁAconst�Œl���ω����Ȃ��ؖ�(��ʂɑ��݂ł���L�������̐���)
    int order[maxdata][2];//spd,number
    int weaponid = 0; //�G���h���b�v����������id




    int itemtype[2] = { 0 };
    int allexp = 0;
    int allgold = 0;

    bool levelupflg = false;

    bool fastsetflg = true;

    static int image;
    static int background;

public:
    // �R���X�g���N�^�Ŕz������ׂ� -1 �ŏ�����
    MatchManager()
    {
        for (int i = 0; i < maxdata; ++i) {
            order[i][0] = -1;
            order[i][1] = -1;
        }
    }
    //�f�X�g���N�^.
    ~MatchManager() {}

    //�Z�b�^�[.
    void setAnyFlg(bool newflg) { anyflg = newflg; }
    void setAnyNumber(int newnum) { anynumber = newnum; }

    void addAnyNumber() { anynumber++; }
    void setDataCnt(int newDataCnt) { datacnt = newDataCnt; }
    void addPlayerCnt() { playercnt++; }
    void addEnemyCnt() { enemycnt++; }
    void addTurn() { turn++; }//�^�[����+1���������Ȃ��̂ł���p��.
    void setTurn(int newTurn) { turn = newTurn; }
    void addCurrentTurn() { currentturn++; }//�^�[���̒��ŒN���s���������Ǘ�����.
    void setCurrentTurn(int newcurrentturn) { currentturn = newcurrentturn; }
    void setAtk(int newatk, int newskillatk, int newelement, int newskillflg) { atk = newatk; skillatk = newskillatk;  atkelement = newelement; skillflg = newskillflg; }//�U�����̊e��f�[�^���擾
    void setDef(int newdef, int newelement) { def = newdef; defelement = newelement; }//�h��́A�����̎擾
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

    //�Q�b�^�[(const�����Ēl��ύX���Ȃ����Ƃ̏ؖ�).
    bool getAnyFlg() const { return anyflg; }
    int getAnyNumber() const { return anynumber; }
    bool getGameFlg() const { return gameflg; }
    int getWinnerFlg() const { return winnerflg; }
    bool getLogNumberFlg(int number) const { return lognumberflg[number]; }
    int getData() const { return datacnt; }
    int getPlayerCnt() const { return playercnt; }
    int getEnemyCnt() const { return enemycnt; }
    int getTurn() const { return turn; }
    int getOrder(int receiveOrder)const { return order[receiveOrder][1]; }//Order�̒l�����Q��.
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

    int Atack(int action)  //�U���̈З͂̌v�Z�A���M
    {

        int dmg = 0;
        //atk+�X�L���U����+(+-5�̗���)*�����{��-�h���
        if (skillatk >= 0)//�U���X�L���Ȃ�
        {
            //�U���́@�{�X�L���З́@�{(�����@�[�h��́@)*����
            dmg = ((atk + skillatk + (GetRand(randomnumber) - randomnumber / 2) - def)) * element[defelement][atkelement];//�h�䑤�̍U�����ɑ΂���l

            if (dmg <= 0)
            {
                dmg = 1;
            }


        }

        else if (action == 2 || skillatk < 0)//�񕜃X�L���Ȃ�
        {

            dmg = skillatk - (GetRand(randomnumber));

            if (dmg >= 0)
            {
                dmg = -1;
            }
        }
        else if (action == 3)//�񕜓���Ȃ�
        {

            dmg = skillatk;

        }


        return dmg;
    }
    bool Stay(int staytime)//  ��莞�ԑҋ@����
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
    bool onlyStay(int staytime)//  ��莞�ԑҋ@����(��񂫂�)
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


    // �z�񂪋�Ȃ�f�[�^�̕ۑ� 0:�f�����A1,����U��ꂽ�ԍ�
    void setSpdAndOrder(int setspd, int setorder)
    {
        if (order[datacnt][0] == -1 && order[datacnt][1] == -1)
        {
            order[datacnt][0] = setspd;
            order[datacnt][1] = setorder;
            datacnt += 1;
        }
    }


    //�擾�����f�[�^��f�����̏��Ԃɕ��בւ���
    void orderSelect()
    {
        //��������[�v����
        for (int i = 0; i < datacnt;)//datacnt�͂������L���������J�E���g���Ă���.
        {
            //����1���2�̕��������̂Ȃ�
            if (order[i][0] < order[i + 1][0])
            {
                //����p�ӂ��ē���ւ�.
                int changespd, changorder;
                changespd = order[i][0];
                changorder = order[i][1];

                order[i][0] = order[i + 1][0];
                order[i][1] = order[i + 1][1];

                order[i + 1][0] = changespd;
                order[i + 1][1] = changorder;

                i = 0;//����ւ����s������ŏ�����Ċm�F.
            }
            else i++;
        }
    }




    //�s����������������ׂ�.
    bool ChackCurrentTurn()
    {
        return(currentturn > datacnt);
    }

    //�R�}���h�ł��\�����������{���F�̘g�̕ύX.
    void DrawFrame(int x1, int y)
    {
        int addwindowsize = 30;//�傫���̒���
        unsigned int Color[4] = {
           GetColor(255, 255, 255), //�g
           GetColor(0,0,0),//��
           GetColor(0,255,0),//HP�o�[��
           GetColor(255,0,0),//Hp�o�[��

        };

        DrawBox(x1, y, x1 + (playercnt * Chara::size), y + Chara::size * 2, Color[1], true);//�g��
        DrawBox(x1, y, x1 + (playercnt * Chara::size), y + Chara::size * 2, Color[0], false);//�g


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

    // �z��̓��e��\������֐��i�f�o�b�O�p�j
    void showData()
    {
        // �\������ʒu�̏������W (x, y) ��ݒ�
        int posX = 1000;  // X���W
        int posY = 10;   // Y���W (�����l)

        for (int i = 0; i < datacnt; ++i)
        {
            // turn[i] ��2�̒l�i�f�����Ə��ԁj�����ꂼ��\��
            char buffer[100];
            snprintf(buffer, sizeof(buffer), "turn[%d]: SPD = %d, Order = %d", i, order[i][0], order[i][1]);

            // ��ʂɕ������`�� (x, y���W, �F)
            DrawString(posX, posY, buffer, GetColor(255, 255, 255));

            // �`��ʒu�����̍s�Ɉړ�
            posY += 20;  // 20�s�N�Z�����Ɉړ�
        }

        DrawString(0, 600, std::to_string(getCurrentTurn()).c_str(), 0x00ff00);//1�^�[���̒��ŉ��l�s����.
        DrawString(0, 630, std::to_string(getData()).c_str(), 0x00ff00);//1�^�[���̒��ŉ��Ԗڂ̍s����.
    }

    void risetAllData()//�S�f�[�^�̍폜.
    {
        datacnt = 0;//�S���̓G�A�����̐�.
        playercnt = 0;
        enemycnt = 0;
        currentturn = 0;//���ݒN�̃^�[�����̃J�E���g.
        turn = 0;//�o�߂����^�[�������̃J�E���g

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
            printfDx("�摜�̓ǂݍ��݂Ɏ��s���܂���: %s\n", ImagePath);
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

    //���L��͂��܂肫�ꂢ����Ȃ��C������B��񂫂�̓���ɂ͂�����肽����
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
    static const int limitlist = 10;//�ۑ��ł���A�G�A�v���C���[�̍ő吔(�}�b�`manager�ɂ���maxdata�Ƃ��Ԃ�)
    std::vector<std::vector<int>> charalist;

    //  int charalist[2][limitlist] = { 0 };//���ׂď���������!!!
    bool selecter = false;//�G��I������^�[����
    //  static const int staytime = 10;//�I���J�[�\���̑ҋ@����

    int cursorpos = 0;//���݂̃Z���N�^�[�̏ꏊ(�G�̑I��p).
    int commandpos = 0;//�Z�I���̃J�[�\��(�R�}���h�I���̂��߂ɕ������Ŏg�p)
    int commandcursor = 0;//�Z�I���̕`��p�Ɏg�p
    int whichcharacters = 0;//�N�̃R�}���h��`�悷�邩.
    int range = 3;//�R�}���h�����`�悷�邩0~3(�l���ς��ƑΉ��ł��Ȃ�)
    int ordernumber = 0;//�R�}���h�̏����擾����Ƃ��̏Z����ۑ�
    int enyraction = 0;//�s�����ǂ�ɓ����邩(0,�U��1,���Z2,�ǂ���3,�h��4,�ɂ���)
    int addres = 0;//�R�}���h�̕`��

    const char* action[20] = { "��������", "�Ƃ���", "�ǂ���", "�ɂ���", "5", "6", "7", "8", "9", "10", "11", "12", "13" };




    //�U���́Askill�̈З́A�Z����

public:

    //�R���X�g���N�^
    CommandSelector() {
        charalist.resize(2);

    }
    //�f�X�g���N�^
    ~CommandSelector() {}


    //�Q�b�^�[.
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

    //�Z�b�^�[.
    void setX(int newx) { x = newx; }
    void setY(int newy) { y = newy; }
    void setSelecter(bool newselecter) { selecter = newselecter; }
    void setCursorPos(int newcursorpos) { cursorpos = newcursorpos; }

    void setWhichCharacters(int newCharacters) { whichcharacters = newCharacters; };

    void setOrderNumber(int newnumber) { ordernumber = newnumber; }




    void setCharaList(int Pnumber, int Enumber)
    {
        // �v���C���[�ԍ���charalist[0]�ɒǉ�
        if (Pnumber != -1) {
            charalist[0].push_back(Pnumber);
        }

        // �G�ԍ���charalist[1]�ɒǉ�
        if (Enumber != -1) {
            charalist[1].push_back(Enumber);
        }
    }




    bool chackEnemyList(int PorE, int number) {//�E�ɐi��ł�����

        return (charalist[PorE].size() > number + 1);

    }

    int getSizeReserve(int num)//�G�l�~�[�L���������݉��̂��邩���ׂ�0�������A1���G
    {
        return charalist[num].size() - 1;
    }



    //commandpara�̃T�C�Y���m�F����
    int getArraySize(const std::vector<std::vector<std::string>>& array, int number) {
        if (number >= 0 && number < array.size()) {//�����Ƒ��݂���͈͂𒲂ׂ邩����.

            return array[number].size();
        }
        else {
            return 0;
        }
    }



    //�R�}���h�I�����͈͊O�̃A�h���X�ɍs�����Ⴄ�I
    void UpdateCursor(int direction, int arraysize)
    {
        // -1�܂���1�ȊO�̒l��������J�[�\���̐ݒ�����Z�b�g
        if (direction != -1 && direction != 1)
        {
            commandpos = 0;
            addres = 0;
            commandcursor = 0;
            return;
        }

        // �z��T�C�Y�� 0 �̏ꍇ�͏��������Ȃ�
        if (arraysize <= 0) return;

        // commandcursor ���X�V
        commandcursor += direction;

        // arraysize �� 4 �����̏ꍇ�͐���������������
        int cursorLimit;
        if (arraysize < 4)
        {
            cursorLimit = arraysize - 1;
        }
        else
        {
            //4�ȏ�Ȃ�]���ʂ�
            cursorLimit = 3;
        }


        // commandcursor �� 0 �����̏ꍇ�Aaddres �����炷
        if (commandcursor < 0)
        {
            if (addres > 0)
            {
                addres--;
                commandcursor = 0;  // commandcursor �� 0 �����ɂȂ�Ȃ��悤�ɒ���
            }
            else
            {
                commandcursor = 0;
            }
        }

        // commandcursor �������l (cursorLimit) �𒴂����ꍇ�Aaddres �𑝂₷
        if (commandcursor > cursorLimit)
        {
            if (arraysize >= 4 && addres < arraysize - 4) // �z��T�C�Y�� 4 �ȏ�̂Ƃ��̂݃X�N���[��
            {
                addres++;
                commandcursor = cursorLimit;
            }
            else
            {
                commandcursor = cursorLimit;
            }
        }

        // �ŏI�I�� commandpos �̌v�Z�icommandcursor + addres�j
        commandpos = commandcursor + addres;

        // �z��T�C�Y�𒴂��Ȃ��悤�ɐ���
        if (commandpos >= arraysize)
        {
            commandpos = arraysize - 1;
        }
    }


    void DrawCommand(const std::vector<std::vector<std::string>>& commandpara, const std::vector<std::vector<int>>& usemppoint, int(&Itemnum)[256][2], int number, int size)
    {
        /*
        DrawString(0, 130, (std::string("�X�^�[�g�|�X:") + std::to_string(commandpos)).c_str(), 0xff0fff);
        DrawString(0, 150, (std::string("�J�[�\��;") + std::to_string(commandcursor)).c_str(), 0xff0fff);
        DrawString(0, 170, (std::string("�A�h���X:") + std::to_string(addres)).c_str(), 0xff0fff);
        */
        int x = 950, y = 450;
        int x2 = 200;//�R�}���h�̂ƂȂ�ɕ\������MP��Pos
        int pos = 0;//command�̕`��p

        //�����R�}���h���\���ł��鐔�ȏ�Ȃ�
        if (size > range)
        {
            size = range + 1;
        }

        //command�̕\��
        for (int i = 0; i < size; i++)
        {
            DrawString(x, y + pos * 20, commandpara[number][i + addres].c_str(), 0xffffff);

            //�Ƃ����ꗗ��\���̎���
            if (number != commandpara.size() - 1 && number != commandpara.size() - 2)
            {
                DrawString(x + x2, y + pos * 20, ("MP" + std::to_string(usemppoint[number][i + addres])).c_str(), 0xffffff);
            }
            else if (number != commandpara.size() - 1)
            {

                //    DrawFormatString(x + x2, y + pos * 20, 0xffffff, "%d��",  player.GetPosItem(i, 1));

                DrawFormatString(x + x2, y + pos * 20, 0xffffff, "%d��", Itemnum[i][1]);

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
        //�����Ȃ�
        return true;

        //�����Ȃ�

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



    /*�����ł��B�z��̗v�f���K�v�Ȏ��A�����Ɏg�p����^�͂ǂ�Ȃ��̂�������
                  �����b�g	                   �f�����b�g
    �l�n��	   ���̔z��ɉe����^���Ȃ��B 	�z��̃R�s�[�������B
     const &   �R�s�[���s�v�Ō����I�B    	���̔z��ւ̎Q�Ƃ��K�v�B
    */
    void checkCanShift(int target, int direction, std::vector<Chara>& Charas)
    {

        //M�Ń��Z�b�g����ƉE�ɍs��������
        int num = 1;
        // �����Ă���L�����̌���
        switch (direction)
        {
        case 1://D

            //�����Ă���L�����̌���.
            while (Charas[getCharaList(target, getCursorPos() + num)].getHp() == 0 &&
                chackEnemyList(target, getCursorPos() + num))
            {
                num++;
            }

            //num���͈͊O�ɂȂ�Ȃ��悤�Ƀ`�F�b�N.
            while (Charas[getCharaList(target, getCursorPos() + num)].getHp() == 0)
            {
                num--;
            }

            //���������A�h���X�Ɉړ�.
            setCursorPos(getCursorPos() + num);

            break;

        case -1://A
            //�����Ă���L�����̌���.
            while (Charas[getCharaList(target, getCursorPos() - num)].getHp() == 0 &&
                getCursorPos() - num > 0)
            {
                num++;
            }
            while (Charas[getCharaList(target, getCursorPos() - num)].getHp() == 0)
            {
                num--;
            }
            //���������A�h���X�Ɉړ�.
            setCursorPos(getCursorPos() - num);

            break;
        }
    }



    int const checkStringOrInt(int x, int y, std::vector<std::vector<std::string>> commandpara)//0�͍U��,1�͂Ƃ���,2�͂ǂ���,3������
    {
        /*
        1.�U��
        2.���Z�̑I��
        3.����̑I��
        4.�h��
        5.������
        */


        // printfDx("%s", commandpara[x][y].c_str());

        try {//�G���[���o���珈���𒆎~����.
            std::stoi(commandpara[x][y]);  // �ϊ��ł���ꍇ�A����
            return  0;//�U��
        }
        catch (const std::invalid_argument&) {
            // �ϊ��ł��Ȃ��ꍇ�i������Ƃ��Ĉ����j

            if (commandpara[x][y] == "��������")
            {
                return 0;
            }
            if (commandpara[x][y] == "�Ƃ���")
            {
                return 2;
            }
            else if (commandpara[x][y] == "�ǂ���")
            {
                return 3;
            }
            else if (commandpara[x][y] == "�ڂ�����")
            {
                return 4;
            }
            else if (commandpara[x][y] == "�ɂ���")
            {
                return 5;
            }
            else
            {
                return 1;//��L�ȊO�͓��Z���ɓ�����
            }
        }
    }
};

//�{�^���̓��͂ɂ̂ݎg�p,
class InputManager {
private:
    //�e�L�[�̃{�^���������ꂽ�u�Ԃ��ǂ������ʂ��邽�߂Ɏg�p.
    static const int KEY_CODE_MAX = 256;
    bool hitFlags[KEY_CODE_MAX] = { false };  // �e�L�[�̉�����Ԃ�ێ�����z��.

public:
    // �R���X�g���N�^.
    InputManager() {}

    // �f�X�g���N�^.
    ~InputManager() {}

    // �w�肵���L�[�R�[�h�������ꂽ�u�Ԃ���true��Ԃ�.
    bool CheackAnyKeyDown(int keyCode) {
        // �L�[�R�[�h���͈͓��ł��邩�m�F.
        if (keyCode < 0 || keyCode >= KEY_CODE_MAX) {
            return false;  // �͈͊O�̏ꍇ��false ��Ԃ�.
        }

        // �L�[��������Ă��āA�Y���L�[�̃t���O��false�̏ꍇ.
        if (CheckHitKey(keyCode) && !hitFlags[keyCode]) {
            hitFlags[keyCode] = true;  // �����ꂽ�u�ԂɃt���O�� true �ɂ���.
            return true;               // �����ꂽ�u�Ԃ̂� true ��Ԃ�.
        }
        else if (!CheckHitKey(keyCode)) {
            hitFlags[keyCode] = false;  // �L�[�������ꂽ��t���O�����Z�b�g.
        }
        return false;  // �������ςȂ��A�܂��͉�����Ă��Ȃ��ꍇ�� false.
    }
    // �w�肵���L�[�R�[�h��������Ă����true��Ԃ�.
    bool CheackAnyKeyHold(int keyCode) {
        // �L�[�R�[�h���͈͓��ł��邩�m�F.
        if (keyCode < 0 || keyCode >= KEY_CODE_MAX) {
            return false;  // �͈͊O�̏ꍇ��false ��Ԃ�.
        }

        // �L�[��������Ă��āA�Y���L�[�̃t���O��false�̏ꍇ.
        if (CheckHitKey(keyCode)) {
            return true;
        }
        else return false;//������Ă��Ȃ��ꍇ�� false.
    }




};


void sentou();
void allDelete();
void setEnemys(int);

#endif
