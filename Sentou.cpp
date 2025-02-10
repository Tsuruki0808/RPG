#include "DxLib.h"
#include "sentou.h"
#include "GameManeger.h"
#include "Data.h"

#include <iostream>
#include <algorithm> //�z������̒l�ł��ׂď��������邽�߂Ɏg�p.
#include <vector>
#include <string>


//�N���X�̓w�b�_�[�ɋL��


//�O���[�o���ϐ��A�֐�
static int size;
int Chara::size = 128;//�摜�T�C�Y
int Chara::image[Chara::totalImages] = { -1 };
int MatchManager::background = 0; // background�̏�����
int MatchManager::image = -1;

float MatchManager::element[16][10] = { //�����\�Ɏg�p [�U����][�h�䑤](�X�̔z��̐��̓L�������̕ϐ��ɂ���������)
    // �����@ ���@ �X�@ ���@ ���@ ��
      {   1,   1,   1,   1,   1,   1, 2},//�v���C���[
      {   1, 0.5,   1,   1,   2,   2, 2},//�X�P���g��
      { 1.5,   1,   2,   1,0.75,   2, 2},//�e�T
      {0.25,   1,   1,   1,   1,   3, 2},//�ח�
      {0.75,   2,   1, 1.5, 0.5, 1.5, 2},//���ꂽ��
      {0.75,   2, 0.5,   1,   1,   1, 2},//�S�҂̌��m
      {   1,   1,   1,   1,   1,   1, 2},//���c�M��
      { 0.5, 0.5,   2,0.75,   1, 1.5, 2},//����X���C��
      {   1,   1,0.75,   1,   2, 0.5, 2},//�ٌ`�̖���
      {0.75,   2,   1, 1.5,0.75, 1.5, 2},//���s�����G��
      {   1,0.75,   1,   2,   1, 0.5, 2},//�א_���ő�
      { 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 2},//���^�S�[����
      { 1.5, 0.5, 0.5, 0.5, 0.5,0.25, 2},//�ł̎i��
      {0.25,0.25,0.25,0.25,0.25,0.25, 2},//�O���r�e�B�t���b�v�N
      {   3,   3,   3,   3,   3,   3, 2} 
};


enum Action
{
    ACTION_ATTACK = 0,//��������
    ACTION_SKILLATTACK = 1,//�Ƃ����ł�������
    ACTION_SKILL = 2,//�Ƃ���
    ACTION_TOOLS = 3,//�ǂ���
    ACITON_DEFENSE = 4,//�h��
    ACITON_ESCAPE = 5//������
};


bool Sentouflg;//���ݐ퓬�����ǂ����̃t���O.
bool fasttime = true;//�{���̏���N�����Ɏ��s
int Itemnum[256][2] = { 0 };//Id, ��
  

std::vector<Chara> Charas;//�L�����I�u�W�F�N�g�̔z��.
std::vector<std::vector<int>> numbers(1, std::vector<int>(8));//�L�����̃f�[�^��z��ɓ���邽�߂ɐ������锠.

std::vector<std::vector<int>> atkpara;//�U���͂����̓񎞌��z��ɓ����
std::vector<std::vector<int>> elementpara;//���������̓񎞌��z��ɓ����

std::vector<std::vector<int>> usemppoint;//����MP�����̓񎞌��z��ɓ����

std::vector<std::vector<bool>> skillpara;//�S�̍U�����ۂ����̓񎞌��z��ɓ����
std::vector<std::vector<std::string>> commandpara;//�R�}���h�����̓񎞌��z��ɓ����

std::vector<std::string> loghistory;//�퓬�̏󋵂�������郍�O�p�̔z��

//�G�͉��L�̔z��ɓ���邱�ƂŐ���
std::vector<int> Enemys = {};//7.8.9.10



//�N���X�̍쐬
MatchManager Manager;
CommandSelector C_Selecter;
InputManager InputHandler;

//�퓬�Ɋւ���f�[�^�����ׂă��Z�b�g
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

    //�R���X�g���N�^���Ăяo���ă��Z�b�g����.
    Manager = MatchManager(); 
    C_Selecter = CommandSelector(); 
    InputHandler = InputManager();

    Sentouflg = true;
}

//�퓬���I�����
void endGame(int number)
{
    if (number == 1)//����
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
    //����
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

//�G�L�����̃Z�b�g
void setEnemys(int number) { Enemys.push_back({ number + 1 }); }



void sentou()
{
    //����N�����ɕK�v
    if (fasttime)
    {
       // printfDx("����N�����̓ǂݍ���\n");
        Chara::LoadImage("��l��2.png");//�ʂ̂Ƃ��ň�񂫂�̓ǂݍ��݂ɂ�����

        Manager.LoadImage("textwindow.png");//�g�̕ۑ�

        Manager.LoadImageBackground("sougen_back.png");

        fasttime = false;

    }


    if (Sentouflg)//�퓬�t���O���^�Ȃ�.
    {
        //�e��ݒ�̏�����
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
            //�v���C���[�̐����[�v(����͈�݂̂̂����炢����)
            numbers.push_back(std::vector<int>{player.GetId(), player.GetMaxHp(), player.GetMaxMp(), player.GetStr(), player.GetDef(), player.GetAgi(), player.GetLevel(), player.GetId()});//�ԍ�,hp,mp,atk,def,spd

            //�v���C���[�̏���ۑ�����ꏊ���m��
            commandpara.push_back({ });
            atkpara.push_back({ });
            usemppoint.push_back({ });
            elementpara.push_back({ });
            skillpara.push_back({ });

            for (int i = 1; i <= player.GetSkillCnt() - 1; i++)
            {
                // printfDx("�X�L��%d:%s\n", i, skill[i].GetName().c_str());

               //  printfDx("�З�%d:%d\n", i, skill[i].GetDmg());

                 //�R�}���h�̏��(�Z��)
                commandpara[commandpara.size() - 1].push_back({ skill[i].GetName() });

                //�R�}���h�̏��(����)
                elementpara[elementpara.size() - 1].push_back({ skill[i].GetEle() });

                //�R�}���h�̏��(�U����)
                atkpara[atkpara.size() - 1].push_back({ skill[i].GetDmg() });

                //�R�}���h�̏��(����MP)
                usemppoint[usemppoint.size() - 1].push_back({ skill[i].GetMp() });

                //�S�̍U�����ۂ�(�^�Ȃ�S�̍U��)
                skillpara[skillpara.size() - 1].push_back({ skill[i].GetFlg() });

            }


        }


        //�G��MP������Ȃ����ǃt�H�[�}�b�g�ɍ��킹�邽�߂ɍ쐬
        usemppoint.push_back({ 0,0,0,0 });

        int i = 0;
        for (int j = 0; j < Enemys.size(); j++)
        {//�ԍ�,hp,mp,atk,def,spd,level, element(�V�K�ǉ�)

            numbers.push_back(std::vector<int>{enemy[Enemys[i]].GetId(), enemy[Enemys[i]].GetHp(), enemy[Enemys[i]].GetMp(), enemy[Enemys[i]].GetStr(), enemy[Enemys[i]].GetDef(), enemy[Enemys[i]].GetAgi(), enemy[Enemys[i]].GetLevel(), enemy[Enemys[i]].GetId()});//�ԍ�,hp,mp,atk,def,spd

            //�R�}���h���̎擾
            commandpara.push_back({ enemy[Enemys[i]].GetSkillName(0), enemy[Enemys[i]].GetSkillName(1), enemy[Enemys[i]].GetSkillName(2), enemy[Enemys[i]].GetSkillName(3) });   // ������̔z��

            //�Z�̈З͂̎擾
            atkpara.push_back({ enemy[Enemys[i]].GetSkillDmg(0) , enemy[Enemys[i]].GetSkillDmg(1), enemy[Enemys[i]].GetSkillDmg(2), enemy[Enemys[i]].GetSkillDmg(3) });

            //�����̎擾
            elementpara.push_back({ enemy[Enemys[i]].GetSkillEle(0) , enemy[Enemys[i]].GetSkillEle(1), enemy[Enemys[i]].GetSkillEle(2), enemy[Enemys[i]].GetSkillEle(3) });

            //�S�̍U�����ۂ�(�^�Ȃ�S�̍U��)
            skillpara.push_back({ enemy[Enemys[i]].GetSkillFlg(0) , enemy[Enemys[i]].GetSkillFlg(1), enemy[Enemys[i]].GetSkillFlg(2), enemy[Enemys[i]].GetSkillFlg(3) });

            i++;
        }



        int cnt = 0;
        for (int i = 0; i < 256; i++)
        {
            //printfDx("%d�Ԗڂ�ID%d\n", i, item[i].GetId());

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


        //�A�C�e���̏���ۑ�����ꏊ���m��
        commandpara.push_back({});
        atkpara.push_back({ });
        elementpara.push_back({ });
        //�A�C�e����MP������Ȃ����ǃt�H�[�}�b�g�ɍ��킹�邽�߂ɍ쐬
        usemppoint.push_back({  });
        skillpara.push_back({ });

        for (int i = 0; i < cnt; i++)
        {
            //�A�C�e�����(���O)
            commandpara[commandpara.size() - 1].push_back({ item[Itemnum[i][0] - 2000].GetName() });

            //�A�C�e�����(�U����)
            atkpara[atkpara.size() - 1].push_back({ -item[Itemnum[i][0] - 2000].GetHp() });

            //�A�C�e���̏��(����)
            elementpara[elementpara.size() - 1].push_back({ 0 });

            //���L2�͎g�p���Ȃ����ǖ��߂Ȃ��Ƃ����Ȃ�
            usemppoint[usemppoint.size() - 1].push_back({ 0 });
            skillpara[skillpara.size() - 1].push_back({ 0 });


        }


        //�A�C�e���̉񕜂͌Œ�l�A�G��|���Ɣz��O��(�E�����Ԗ�)MP�̓����AHP�A�ő�HP�𕪂���B


        //�Œ�̃R�}���h
        commandpara.push_back({ "��������", "�Ƃ���", "�ǂ���", "�ڂ�����", "�ɂ���" });//���Ɋ�{�̃R�}���h


        C_Selecter.setWhichCharacters(commandpara.size() - 1);//���̃R�}���h���Z�b�g(�R�}���h�Z���N�g�̂��Ƃ�)


        //�z��̃f�[�^�����ƂɃL�����N�^�[�̐���.
        for (size_t i = 0; i < numbers.size(); ++i) {
            if (numbers[i][6] != 0)//���x����0�łȂ��Ȃ�(�v���C���[�Ȃ�).
            {
                Charas.push_back(Chara(100 + (Manager.getPlayerCnt() * Chara::size), 400, numbers[i][0], numbers[i][1], numbers[i][2], numbers[i][3], numbers[i][4], numbers[i][5], numbers[i][6], numbers[i][7], i));
                Manager.addPlayerCnt();
                C_Selecter.setCharaList(i, -1);//�v���C���[�̔ԍ���ۑ�

                //�����Ō��݂�HP�AMP��K�p
                Charas[i].setHp(player.GetHp());
                Charas[i].setMp(player.GetMp());
                // player.SetHp()
            }
            else if (numbers[i][6] == 0)//�G�l�~�|�Ȃ�
            {
                Charas.push_back(Chara(150 + (Manager.getEnemyCnt() * Chara::size), 100, numbers[i][0], numbers[i][1], numbers[i][2], numbers[i][3], numbers[i][4], numbers[i][5], numbers[i][6], numbers[i][7], i));
                Manager.addEnemyCnt();
                C_Selecter.setCharaList(-1, i);//�G�l�~�[�̔ԍ���ۑ�
            }

        }


        for (auto& c : Charas)//�z��ɑf�����A�ԍ������ŕۑ�
        {
            Manager.setSpdAndOrder(c.getSpd(), c.getOrder());
        }

        //1��ǂݍ��߂���t���O��ύX        
        Sentouflg = false;

        Manager.addAnyNumber();



        loghistory.insert(loghistory.begin(), "�����ꂽ�I");//�z��̐擪�̃A�h���X���w�肵�A�퓬�ɗv�f��ǉ�

        while (Manager.getAnyNumber() <= Manager.getEnemyCnt() && Manager.getAnyNumber() >= 0)

            //if (Manager.Stay(20) && Manager.getAnyNumber() <= Manager.getEnemyCnt() && Manager.getAnyNumber() >= 0)
        {
            int j = Manager.getAnyNumber();
            if (j < Manager.getEnemyCnt())
            {
                //���O�̕\��
                loghistory.insert(loghistory.begin(), enemy[Charas[C_Selecter.getCharaList(1, j)].getNumber()].GetName() + "");//�z��̐擪�̃A�h���X���w�肵�A�퓬�ɗv�f��ǉ�
            }
            else
            {
            }

            Manager.addAnyNumber();


        }

        //�G�̏o�����O�̕\��

    }
    
    
 
   


    
    if (!Manager.getGameFlg())//���s�����܂����Ȃ�Ȃ�
    {
        //�~�������ƃ{�^������������i�ނ悤�ɂ������ˁB
        //�퓬���̃��O�̓I�[�g�ł�������
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
                if (Manager.getWinnerFlg() == 1)//����
                {
                    
                    loghistory.insert(loghistory.begin(), player.GetName() + "�̏����I�I");
                }
                else if (Manager.getWinnerFlg() == -1)//�܂�
                {
                    loghistory.insert(loghistory.begin(), "�S�ł��Ă��܂���...");
                    
                }
                else if (Manager.getWinnerFlg() == 0)
                {
                    loghistory.insert(loghistory.begin(), player.GetName() + "�͓�����...");

                }
                else
                {
                    loghistory.insert(loghistory.begin(), "��O����������������������������");
                }
               
                
                break;
            case 1:
                
                if (Manager.getWinnerFlg() == 1)//����
                {
               

                    loghistory.insert(loghistory.begin(), "�o���l" + std::to_string(Manager.getAllExp()) + "�l��!");
                }
                else if (Manager.getWinnerFlg() == -1)//�܂�
                {
                    loghistory.insert(loghistory.begin(), "�ڂ̑O���^���ÂɂȂ���...");
                   
                }
                else if (Manager.getWinnerFlg() == 0)
                {
                    loghistory.insert(loghistory.begin(), "�X�y�[�X�L�[�Ő퓬�̏I��");
                    Manager.setAnyFlg(TRUE);
                }
                else
                {
                    loghistory.insert(loghistory.begin(), "��O����������������������������");
                }
                break;

            case 2:
                if (Manager.getWinnerFlg() == 1)//����
                {
                    if (Manager.getLevelUpFlg() == true)
                    {
                        loghistory.insert(loghistory.begin(), player.GetName() + "�̓��x�����オ�����I");
                         break;
                    }
                    else
                    {
                        number++;
                        Manager.setLogNumberFlg(number, TRUE);
                    }
                }
                else if (Manager.getWinnerFlg() == -1)//�܂�
                {
                    loghistory.insert(loghistory.begin(), "�X�y�[�X�L�[�Ő퓬�̏I��");
                    Manager.setAnyFlg(TRUE);
                    break;
                }
                else
                {
                    loghistory.insert(loghistory.begin(), "��O����������������������������");
                    break;
                }

             
            case 3:
                if (Manager.getWinnerFlg() == 1)//����
                {
                    loghistory.insert(loghistory.begin(), std::to_string(Manager.getAllGold()) + "�S�[���h�̊l��!");
                  
                }
                else if (Manager.getWinnerFlg() == -1)//�܂�
                {

                    loghistory.insert(loghistory.begin(), "�X�y�[�X�L�[�Ő퓬�̏I��");
                    Manager.setAnyFlg(TRUE);
                }
                else
                {
                    loghistory.insert(loghistory.begin(), "��O����������������������������");
                }
                break;
            case 4:
                if (Manager.getWinnerFlg() == 1)//����
                {
                    if (Manager.getWeaponId() == 0 )
                    {
                        number++;
                        Manager.setLogNumberFlg(number, TRUE);
                        
                    }
                    else
                    {
                        loghistory.insert(loghistory.begin(), "�����i�A" + soubi[Manager.getWeaponId()].GetName() + "�̊l��!");
                        break;
                    }
                }
                else if (Manager.getWinnerFlg() == -1)//�܂�
                {
                    allDelete();
                    break;
                }
                else
                {
                    loghistory.insert(loghistory.begin(), "��O����������������������������");
                    break;
                }
                
            case 5:
                loghistory.insert(loghistory.begin(), "�X�y�[�X�L�[�Ő퓬���I�����܂��I");
                Manager.setAnyFlg(TRUE);
                break;
            case 6:
                allDelete();
                break;
            default:
                printfDx("~ERROR~�{�����̕��͏o��mmmmmmmmmmmmmmmmmmmmmm");
                break;
            }
        }

        if (Manager.getAnyFlg() == true)
        {
            DrawString(935, 600, "Space�Ő퓬�̏I��", 0xffffff);
        }
        
        //�������������܂��Ă�����퓬���I���ł���悤�ɂ���(�����͂����Ő퓬�t���O�����Z�b�g)
        if (InputHandler.CheackAnyKeyDown(KEY_INPUT_SPACE) && Manager.getAnyFlg() == TRUE)
        {
            allDelete();

            //changeCurrentState(TITLE);
            changeCurrentState(GAME);

        }
     
        
    }




    //�퓬�̊Ǘ�
    Manager.DrawBackGround(0,100,1400,600);//�����̔w�i�̕\��
    
    Manager.addTurn();//�s�������������^�[����++(�C����)
    Manager.orderSelect();//�f�������Ńf�[�^����ёւ�
    Manager.DrawOnlyFrame(925, 425, 1220, 550);//�R�}���h�t���[���̕`��
    Manager.DrawOnlyFrame(925, 50, 1270, 350);//���O�̘g�̕`��
    


    
    //��������Ȃ�
    if (Manager.getCurrentTurn() > Charas.size() - 1)//CurentTurn��0����v�f�������Ă���̂ŁA-1���Ă��炷.
    {
        Manager.setCurrentTurn(0);
    }


    if (Charas.size())//�z��ɗv�f�������Ă���Ȃ�
    {
        //�g�̕`��
        Manager.DrawFrame(Charas.front().getX(), Charas.front().getY());//���[X�AY

        
    }




    //���O��`�悷��^�C�~���O�BStay()�֐����g��
    //�퓬�I�������B�A�C�e���l���̃^�C�~���O
    //���܂ɃR�}���h��I��������z��O�ɃA�N�Z�X�����Ⴄ

    //���O�̕`��
    for (int i = 0; i < 10; i++)
        //for (int i = 0; i < loghistory.size(); i++)
    {

        if (loghistory.size() > i)
        {
            DrawString(950, 100 + i * 20, loghistory[i].c_str(), 0xffffff);

        }
    }
    

    //�G�̍U������
    if (Manager.getEnemyFlg() == true)
    {
        //�����҂��Ă���U��
        if (Manager.Stay(30) && Manager.getGameFlg())
        {
            //�ǂ̃v���C���[���U�����邩�̒��I������s��
            int atknumber;

            bool target = 0;//0���v���C���[�A1���G�l�~�[.

            if (Manager.getSkillAtk() >= 0)
            {
                atknumber = GetRand(C_Selecter.getSizeReserve(0));//�����ł�������͈͂��߂�
                target = 0;
                int cnt = 0;
                if (Manager.getSkillFlg())//�S�̍U�����ǂ����̒��I
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
                    loghistory.insert(loghistory.begin(), "����" + std::to_string(ave) + "�̃_���[�W�I");
                    for (int i = 0; i <= C_Selecter.getSizeReserve(0); i++)
                    {
                        if (Charas[C_Selecter.getCharaList(target, i)].getHp() <= 0 && Charas[C_Selecter.getCharaList(target, i)].getDeathFlg() == false)
                        {
                            loghistory.insert(loghistory.begin(), player.GetName() + "�͓|�ꂽ!");
                            Charas[C_Selecter.getCharaList(target, i)].setDeathFlg(TRUE);
                        }
                    }

                }
                else //�P�̍U���Ȃ�
                {

                    while ((Charas[C_Selecter.getCharaList(target, atknumber)].getHp() <= 0 || Charas[C_Selecter.getCharaList(target, atknumber)].getLevel() == 0) && cnt < 1000)//�̗͂�0�ȏ�Ńv���C���[�Ȃ�(���̏ꍇ�̓��x��0�͖���)
                    {
                        cnt++;
                        //�G�l�~�[�Ɠ����֐���
                        atknumber = GetRand(C_Selecter.getSizeReserve(0));
                    }

                    Manager.setDef(Charas[C_Selecter.getCharaList(target, atknumber)].getDef(),
                        Charas[C_Selecter.getCharaList(target, atknumber)].getElement());

                    int dmg = Manager.Atack(C_Selecter.getEnyrAction());
                    Charas[C_Selecter.getCharaList(target, atknumber)].setHp(Charas[C_Selecter.getCharaList(target, atknumber)].getHp() - dmg);
                    loghistory.insert(loghistory.begin(), player.GetName() + "��" + std::to_string(dmg) + "�̃_���[�W�I");
                    if (Charas[C_Selecter.getCharaList(target, atknumber)].getHp() <= 0 && Charas[C_Selecter.getCharaList(target, atknumber)].getDeathFlg() == false)
                    {
                        loghistory.insert(loghistory.begin(), player.GetName() + "�͓|�ꂽ!");
                        Charas[C_Selecter.getCharaList(target, atknumber)].setDeathFlg(TRUE);

                    }

                }

                Manager.risetAtackData();
            }
            else //��
            {
                target = 1;//�W�I�𖡕���              
  
                int Rand = GetRand(99);
                //70%�̊m���Ŏ���Ă��閡������
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
                    loghistory.insert(loghistory.begin(), enemy[Charas[C_Selecter.getCharaList(target, EnemyDate[0])].getNumber()].GetName() + "��" + std::to_string(dmg *= -1) + "�񕜁I");
                }
                else
                {
                    atknumber = GetRand(C_Selecter.getSizeReserve(1));//�����ł�������͈͂��߂�

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

                    loghistory.insert(loghistory.begin(), enemy[Charas[C_Selecter.getCharaList(target, atknumber)].getNumber()].GetName() + "��" + std::to_string(dmg *= -1) + "�񕜁I");

                }
                
              
            }
            //�U������킩��₷��������
            /*
            DrawBox(Charas[C_Selecter.getCharaList(target, atknumber)].getX(), Charas[C_Selecter.getCharaList(target, atknumber)].getY(),
                Charas[C_Selecter.getCharaList(target, atknumber)].getX() + Chara::size, Charas[C_Selecter.getCharaList(target, atknumber)].getY() + Chara::size * 2, 0xff22f0, false);
                */
            //�S�ł�����Q�[���I��
            if (C_Selecter.checkBattleEnds(target, Charas))
            {
                endGame(-1);
            }

            //���̕ӂɕ����̕`������������

            
            
                Manager.setEnemyFlg(false);
                Manager.addCurrentTurn();
            
        }


    }


     if (C_Selecter.getSelecter() == 1 && Manager.getSkillFlg() == false)//�G��I�ԃt���O���^�Ȃ�(�X�̑O�ŃR�}���h�̌���A�U���͂̑��M�͏Z��ł���)
    {
        bool target = 1;//0���v���C���[�A1���G�l�~�[.
        DrawString(935, 600, "A/D�őI���ASpace�Ō���", 0xffffff);

        
        if (Manager.getItemType(0) > 0 || Manager.getItemType(1) > 0 || Manager.getSkillAtk()  < 0)
        {
            target = 0;
            //�J�[�\�����G���疡���ɂȂ�̂ŁA�����Ă��錩����T��(��������s)
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
          //  printfDx("���񂾓G�ɂ����Ă܂����\n");
            //�J�[�\�����G���疡���ɂȂ�̂ŁA�����Ă��錩����T��(��������s)
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


        //�S�̍U���łȂ��Ȃ�J�[�]���̑���ł���(��Ō��؍ς�)
        //�{�^����������ăJ�[�\������ԍ��ɂ��Ȃ��Ȃ�.
        if (InputHandler.CheackAnyKeyDown(KEY_INPUT_A) && C_Selecter.getCursorPos() > 0)
        {//�ׂ�����ł�Ȃ��΂���.
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
            //�ׂ�����ł�Ȃ��΂���.
            if (Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos() + 1)].getHp() == 0)
            {
                C_Selecter.checkCanShift(target, 1, Charas);
            }
            else//�����Ă���Ȃ����炷
            {
                C_Selecter.setCursorPos(C_Selecter.getCursorPos() + 1);
            }
        }

        if (InputHandler.CheackAnyKeyDown(KEY_INPUT_SPACE))
        {

             //�񕜓���Ȃ�
            if (Manager.getItemType(0) > 0)
            {
               
                Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].setHp(Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].getHp() + Manager.getItemType(0));                
                loghistory.insert(loghistory.begin(), player.GetName() + "��HP��" + std::to_string(Manager.getItemType(0)) + "�񕜁I");


            }
            //MP�񕜓���Ȃ�
            else if (Manager.getItemType(1) > 0)
            {
                Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].setMp(Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].getMp() + Manager.getItemType(1));
                loghistory.insert(loghistory.begin(), player.GetName() + "��MP��" + std::to_string(Manager.getItemType(1)) + "�񕜁I");
            }
            else
            {
                
                
                //�̗͂�0�Ȃ�^�[�����΂�������
               //�U�����󂯂鑤�̖h���,�����𑗐M
                Manager.setDef(Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].getDef(), Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].getElement());
                //�����ōU��!
                int dmg = Manager.Atack(C_Selecter.getEnyrAction());
                Manager.risetAtackData();//attack�̃f�[�^���Z�b�g
                Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].setHp(Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].getHp() - dmg);

                //������C�ɂ���
                Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].setEnemyFeeling(-20);
               
                //���O
                if (dmg > 0)
                {
                    loghistory.insert(loghistory.begin(), enemy[Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].getNumber()].GetName() + "��" + std::to_string(dmg) + "�̃_���[�W�I");
                }
                else
                {
                    loghistory.insert(loghistory.begin(), player.GetName() + "��HP��" + std::to_string(-dmg) + "�񕜁I");

                }
                

                //�S�ł�����Q�[���I��
                if (C_Selecter.checkBattleEnds(target, Charas))
                {
                    endGame(1);

                }
                //�G�����񂾂�
                else if (Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].getHp() == 0)
                {
                    //�J�[�\���𐶂��Ă���G�ɂ��炷
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

            C_Selecter.setSelecter(0);//�t���O��false�� 
            Manager.addCurrentTurn();//�U�������̂Ń^�[������            
        }



        //�����̘g���֐��ɂ�������
        /*
        DrawString(Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].getX(), Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].getY(),
            std::to_string(C_Selecter.getCursorPos()).c_str(), 0xff0fff);

        DrawString(0, 400, std::to_string(C_Selecter.getCursorPos()).c_str(), 0xff0fff);
        */
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 54); // �A���t�@�u�����h���[�h��ݒ�A�����x128
        
        DrawBox(Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].getX(), Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].getY(),
            Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].getX() + Chara::size, Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].getY() + Chara::size * 2, 0xff0000, true);
    
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); // �`���Ƀu�����h���[�h��߂�

}
     else if (C_Selecter.getSelecter() == 1 && Manager.getSkillFlg() == true)//�S�̍U��
     {
         bool target = 1;//0���v���C���[�A1���G�l�~�[.
         if (Manager.getSkillAtk() < 0)
         {
             target = 0;
             //�J�[�\�����G���疡���ɂȂ�̂ŁA�����Ă��錩����T��(��������s)
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

             //�̗͂�0�Ȃ�^�[�����΂�������
             //�U�����󂯂鑤�̖h���,�����𑗐M
             Manager.setDef(Charas[C_Selecter.getCharaList(target, i)].getDef(), Charas[C_Selecter.getCharaList(target, i)].getElement());
             //�����ōU��!
             int dmg = Manager.Atack(C_Selecter.getEnyrAction());
             Charas[C_Selecter.getCharaList(target, i)].setHp(Charas[C_Selecter.getCharaList(target, i)].getHp() - dmg);
          
             ave = (ave * (cnt - 1) + dmg) / cnt;
         

             //���ׂĂ̓G����C��
             Charas[C_Selecter.getCharaList(target, i)].setEnemyFeeling(-10);
         }
         if (target == 1)
         {
             loghistory.insert(loghistory.begin(), "����" + std::to_string(ave) + "�̃_���[�W�I");
         }
         else
         {
             loghistory.insert(loghistory.begin(), "����" + std::to_string(ave *= -1) + "�̉񕜁I");
         }
         Manager.risetAtackData();//attack�̃f�[�^���Z�b�g


         //�S�ł�����Q�[���I��
         if (C_Selecter.checkBattleEnds(target, Charas))
         {
             endGame(1);
             
         }
         //�G�����񂾂�
         else if (Charas[C_Selecter.getCharaList(target, C_Selecter.getCursorPos())].getHp() == 0)
         {
             //�J�[�\���𐶂��Ă���G�ɂ��炷
             int i = 0;
             while (Charas[C_Selecter.getCharaList(target, i)].getHp() == 0)
             {
                 i++;
             }
             C_Selecter.setCursorPos(i);
         }

         Manager.risetFastSet(true);
         Manager.setSkillFlg(false);
         C_Selecter.setSelecter(0);//�t���O��false��
         Manager.addCurrentTurn();//�U�������̂Ń^�[������
     }

     //�L�����̓����A�`��.
     for (auto& c : Charas) {




         //�v���C���[�̕`��
         if (c.getLevel() != 0)
         {
             c.Draw();
             c.DrawData(1);//�X�e�[�^�X�̕`��(1�������A0���G).
         }
         else
         {
             bool flg = (c.getHp() != 0);

             //����bool�^��ǉ�
             enemy[c.getNumber()].DrowEnemy(c.getX(), c.getY(), c.getX() + c.getSize(), c.getY() + c.getSize(), flg);

             c.DrawData(0);//�X�e�[�^�X�̕`��(1�������A0���G).
         }


         if (Manager.getGameFlg())//�Q�[�����i�s���Ȃ�
         {

             //���g�̃^�[���Ȃ�(�ԍ�����v���A�U�����I�ԃ^�[���łȂ��Ȃ�)
             if (c.getOrder() == Manager.getOrder(Manager.getCurrentTurn()) && C_Selecter.getSelecter() == 0)
             {
                 DrawString(935, 600, "W/S�őI���ASpace�Ō���AQ�Ŗ߂�", 0xffffff);

                 if (c.getDefenseFlg() == true)
                 {
                     c.setgetDefenseFlg(false);
                     loghistory.insert(loghistory.begin(), player.GetName() + "�͖h����Ƃ���");

                 }

                 if (c.getHp() <= 0)//�̗͂��Ȃ��Ȃ玟�̔Ԃ�
                 {

                     Manager.addCurrentTurn();
                     continue;
                 }
                 //�^�[���������I.�R�}���h�̑I��      
                 DrawBox(c.getX(), c.getY(), c.getX() + Chara::size, c.getY() + Chara::size * 2, 0xfff000, false);


                 if (c.getLevel() != 0)//�v���C���[�Ȃ�R�}���h�I��I��
                 {
                     if (InputHandler.CheackAnyKeyDown(KEY_INPUT_W))
                     {
                         //�J�[�\�������.(����̒ǉ�)
                         C_Selecter.UpdateCursor(-1, C_Selecter.getArraySize(commandpara, C_Selecter.getWhichCharacters()));
                     }

                     if (InputHandler.CheackAnyKeyDown(KEY_INPUT_S))
                     {
                         //�J�[�\��������.             
                         C_Selecter.UpdateCursor(1, C_Selecter.getArraySize(commandpara, C_Selecter.getWhichCharacters()));
                     }

                     /*
                     if (InputHandler.CheackAnyKeyHold(KEY_INPUT_N))
                     {
                         DrawString(0, 300, std::to_string(C_Selecter.getArraySize(commandpara, C_Selecter.getWhichCharacters())).c_str(), 0xff0fff);
                         DrawString(0, 330, commandpara[C_Selecter.getWhichCharacters()][C_Selecter.getCommandPos()].c_str(), 0xffffff);
                         Manager.addCurrentTurn();//�U�������̂Ń^�[������
                     }*/

                     if (InputHandler.CheackAnyKeyHold(KEY_INPUT_Q))//���Ƃ̃R�}���h�ɖ߂�
                     {
                         C_Selecter.UpdateCursor(0, C_Selecter.getArraySize(commandpara, c.getOrder()));//�J�[�\������ԏ��
                         C_Selecter.setWhichCharacters(commandpara.size() - 1);//���Z�b�g
                     }

                     if (InputHandler.CheackAnyKeyDown(KEY_INPUT_SPACE))
                     {
                         //�S�̂Ɍ������s�����ł��Ȃ��A���ʂƋ����Ȃ��čăX�^�[�g


                         //�����őI�������R�}���h���U���Ȃ̂��A�ʂ̃R�}���h�ɑ@�ۂȂ̂����ʂ���(�֐��I����Ă�)
                         int action = C_Selecter.checkStringOrInt(C_Selecter.getWhichCharacters(), C_Selecter.getCommandPos(), commandpara);

                         switch (action)
                         {
                         case ACTION_ATTACK://�U��(�Ƃ����łȂ�)

                             //�I�������R�}���h�̍U���́A�������擾
                            // Manager.setAtk(c.getAtk(), 0, elementpara[c.getOrder()][C_Selecter.getCommandPos()]);//�U���́Askill�̈З́A�Z����
                             Manager.setAtk(c.getAtk(), 0, 0, 0);//�U���́Askill�̈З́A�Z����
                             C_Selecter.setWhichCharacters(commandpara.size() - 1);//���Z�b�g
                             C_Selecter.setSelecter(1);//�G��I�ԃt���O��^�ɂ���.

                             //���O
                             loghistory.insert(loghistory.begin(), player.GetName() + "�̂��������I");


                             break;

                         case ACTION_SKILLATTACK://�Ƃ����̂�������,����̎g�p

                             //����Ȃ�
                             if (C_Selecter.getWhichCharacters() == commandpara.size() - 2)
                             {

                                 //�����ł͓���ɂ����A�N�Z�X���Ȃ��̂ŁA�������ɓ���̏Z���������Ă��܂��B
                                 //�\�Ȃ�X�L���ƍ��킹�����ˁB

                                 //�I�������R�}���h�̍U���́A�������擾
                                // Manager.setAtk(c.getAtk(), atkpara[C_Selecter.getOrderNumber()][C_Selecter.getCommandPos()], elementpara[C_Selecter.getOrderNumber()][C_Selecter.getCommandPos()], skillpara[c.getOrder()][C_Selecter.getCommandPos()]);//�U���́Askill�̈З́A�Z����
                                 Manager.setAtk(0, atkpara[commandpara.size() - 2][C_Selecter.getCommandPos()], 0, skillpara[commandpara.size() - 2][C_Selecter.getCommandPos()]);//�U���́Askill�̈З́A�Z����,�S�̍U�����ǂ���
                                 //HP,MP�̉񕜂����ʂ��邽�߂ɕۑ����Ƃ�


                                 //���L����MP���Ȃ񂩃_��
                                 Manager.setItemType(item[Itemnum[C_Selecter.getCommandPos()][0] - 2000].GetHp(), item[Itemnum[C_Selecter.getCommandPos()][0] - 2000].GetMp());


                                 //Data�̂ق��Ɏg�p����id�𑗐M
                                 player.DecItem(Itemnum[C_Selecter.getCommandPos()][0], 1);


                                 /*
                                 printfDx("���O:%s\n", item[Itemnum[C_Selecter.getCommandPos()][0] - 2000].GetName().c_str());
                                 printfDx("����HP:%d\n", item[Itemnum[C_Selecter.getCommandPos()][0] - 2000].GetHp());
                                 printfDx("����MP:%d\n", item[Itemnum[C_Selecter.getCommandPos()][0] - 2000].GetMp());
                                 */


                                 loghistory.insert(loghistory.begin(), player.GetName() + "��" + commandpara[commandpara.size() - 2][C_Selecter.getCommandPos()].c_str() + "!");


                                 //�����Ō����}�C�i�X
                                 Itemnum[C_Selecter.getCommandPos()][1] = Itemnum[C_Selecter.getCommandPos()][1] - 1;


                                 //��������0�ɂȂ�����B
                                 if (Itemnum[C_Selecter.getCommandPos()][1] == 0)
                                 {
                                     // printfDx("�g���؂��������...\n");
                                      //�e��v�f�̃��Z�b�g

                                     commandpara[C_Selecter.getWhichCharacters()].erase(commandpara[C_Selecter.getWhichCharacters()].begin() + C_Selecter.getCommandPos());
                                     atkpara[C_Selecter.getWhichCharacters()].erase(atkpara[C_Selecter.getWhichCharacters()].begin() + C_Selecter.getCommandPos());

                                     elementpara[C_Selecter.getWhichCharacters()].erase(elementpara[C_Selecter.getWhichCharacters()].begin() + C_Selecter.getCommandPos());
                                     skillpara[C_Selecter.getWhichCharacters()].erase(skillpara[C_Selecter.getWhichCharacters()].begin() + C_Selecter.getCommandPos());
                                     //���L���ړ�����Ƃ����
                                     //   usemppoint[C_Selecter.getWhichCharacters()].erase(usemppoint[C_Selecter.getWhichCharacters()].begin() + C_Selecter.getCommandPos());




                                     for (int i = 0; i < 256; i++)
                                     {
                                         if (Itemnum[i][1] == 0) // ����0�Ȃ�l�߂�
                                         {
                                             for (int j = i; j < 255; j++)
                                             {
                                                 Itemnum[j][0] = Itemnum[j + 1][0];
                                                 Itemnum[j][1] = Itemnum[j + 1][1];
                                                 //printfDx("�ԍ�:%d\n", Itemnum[i][0]);
                                             }


                                         }
                                     }
                                 }
                                 else
                                 {
                                     //  printfDx("�������炵�܂�\n");

                                 }




                                 C_Selecter.setWhichCharacters(commandpara.size() - 1);//���Z�b�g
                                 C_Selecter.setSelecter(1);//�G��I�ԃt���O��^�ɂ���.

                             }
                             //�Ƃ�����I�����AMP������Ȃ�
                             else if (c.getMp() - usemppoint[C_Selecter.getOrderNumber()][C_Selecter.getCommandPos()] >= 0)
                             {
                                 // printfDx("�擾�����X�L���̑���%d\n", elementpara[C_Selecter.getOrderNumber()][C_Selecter.getCommandPos()]);
                                  //�I�������R�}���h�̍U���́A�������擾
                                 Manager.setAtk(c.getAtk(), atkpara[C_Selecter.getOrderNumber()][C_Selecter.getCommandPos()], elementpara[C_Selecter.getOrderNumber()][C_Selecter.getCommandPos()], skillpara[c.getOrder()][C_Selecter.getCommandPos()]);//�U���́Askill�̈З́A�Z����

                                 c.setMp(c.getMp() - usemppoint[C_Selecter.getOrderNumber()][C_Selecter.getCommandPos()]);
                                 //�������


                                 loghistory.insert(loghistory.begin(), player.GetName() + "��" + commandpara[C_Selecter.getOrderNumber()][C_Selecter.getCommandPos()].c_str() + "!");

                                 C_Selecter.setWhichCharacters(commandpara.size() - 1);//���Z�b�g
                                 C_Selecter.setSelecter(1);//�G��I�ԃt���O��^�ɂ���.

                             }
                             else
                             {
                                 loghistory.insert(loghistory.begin(), "�������A" + player.GetName() + "��MP������Ȃ�...");
                                 break;
                             }



                             break;
                         case ACTION_SKILL://�Ƃ���(���Z�̃t�H���_�Ɉڍs)

                             C_Selecter.setWhichCharacters(c.getOrder());
                             //printfDx("�R�}���h�Ɉڍs");



                             C_Selecter.setOrderNumber(c.getOrder());

                             C_Selecter.setEnyrAction(2);
                             break;

                         case ACTION_TOOLS://�ǂ���(����̃t�H���_�Ɉڍs)

                             //������������Ă�����
                             if (Itemnum[0][0] > 0)
                             {
                                 C_Selecter.setWhichCharacters(commandpara.size() - 2);//���Z�b�g
                                 // printfDx("�����I�т܂��傤\n");

                                 C_Selecter.setOrderNumber(commandpara.size() - 2);
                                 C_Selecter.setEnyrAction(3);
                             }
                             else
                             {
                                 loghistory.insert(loghistory.begin(), player.GetName() + "�͓���������Ă��Ȃ�...");
                             }
                             break;

                         case ACITON_DEFENSE://�ڂ�����

                             c.setgetDefenseFlg(TRUE);
                             c.setDef(c.getDef() * 1.5f);

                             loghistory.insert(loghistory.begin(), player.GetName() + "�͎����ł߂�!");

                             Manager.addCurrentTurn();//�U�������̂Ń^�[������
                             //  printfDx("�h��\n");
                             C_Selecter.setEnyrAction(4);
                             break;

                         case ACITON_ESCAPE://�ɂ���
                             
                             //�����̒��I
                             int num = GetRand(10);
                             
                             if (num <= 2)
                             {
                                 loghistory.insert(loghistory.begin(), "�������Ȃ�����...");

                             }
                             else 
                             {
                                 endGame(0);
                             }

                             Manager.addCurrentTurn();//�U�������̂Ń^�[������


                             C_Selecter.setEnyrAction(5);
                             break;
                         }


                         //�J�[�\���̃��Z�b�g.
                         C_Selecter.UpdateCursor(0, C_Selecter.getArraySize(commandpara, c.getOrder()));
                     }
                     C_Selecter.DrawCommand(commandpara, usemppoint, Itemnum, C_Selecter.getWhichCharacters(), C_Selecter.getArraySize(commandpara, C_Selecter.getWhichCharacters()));//�R�}���h�̕`��(�z��𑗂�)
                 }


                 //�G�l�~�[�Ȃ烉���_�����I
                 else if (c.getHp() > 0 && Manager.getEnemyFlg() == false)
                 {


                     //�U���A�񕜃X�L��������U����
                     int AtackSkill[4] = { -1 };//�A�h���X��ۑ�
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
                     //  printfDx("�U��:%d, ��:%d\n", Acnt, Rcnt);


                       //�̗͂����^���̂���O


                       //�����̗̑͂����Ȃ��Ɖ񕜂��₷������
                     for (int i = 0; i <= C_Selecter.getSizeReserve(1); i++)
                     {
                         if (Charas[C_Selecter.getCharaList(1, i)].getHp() <= Charas[C_Selecter.getCharaList(1, i)].getMaxHp() / 2 && Charas[C_Selecter.getCharaList(1, i)].getHp() > 0)
                         {
                             //  printfDx("�̗͂������ȉ��̌��������܂�(�ő�:%d, ����:%d)", Charas[C_Selecter.getCharaList(1, i)].getMaxHp(), Charas[C_Selecter.getCharaList(1, i)].getHp());
                             c.setEnemyFeeling(-30);
                         }
                     }

                     int Rand = GetRand(99);

                     //�U�����������񕜂�������




                     //c.getEnemyFeeling()�͍ő�A�ŏ��l���݂����Ă���

                     //���C�ł���΂���قǉ\���͍���(�񕜋Z�������Ă��Ȃ���΁A�U���Ŋm��)
                     if (Rand <= c.getEnemyFeeling() || Rcnt == 0)
                     {

                         //3:7�ōU��:�Ƃ���(���Z�������Ă��Ȃ��Ȃ�U��)
                         Rand = GetRand(10);
                         if (Rand <= 3 || Acnt == 0)
                         {
                             //�ʏ�U��
                              //���O
                             loghistory.insert(loghistory.begin(), enemy[numbers[c.getOrder()][0]].GetName() + "�̂��������I");
                             Manager.setEnemyFlg(true);
                             Manager.setAtk(c.getAtk(), 0, 0, 0);//�U���́Askill�̈З́A�Z����
                         }
                         else
                         {
                             int i = 0;
                             while (AtackSkill[i] == 0 && i < Acnt)
                             {
                                 i++;
                             }

                             loghistory.insert(loghistory.begin(), enemy[numbers[c.getOrder()][0]].GetName() + "��" + commandpara[c.getOrder()][AtackSkill[i]].c_str() + "�I");
                             Manager.setEnemyFlg(true);
                             Manager.setAtk(c.getAtk(), atkpara[c.getOrder()][AtackSkill[i]], elementpara[c.getOrder()][AtackSkill[i]], skillpara[c.getOrder()][AtackSkill[i]]);//�U���́Askill�̈З́A�Z����

                         }
                         c.setEnemyFeeling(15);
                     }
                     else //��
                     {
                        
                     int i = GetRand(Rcnt);
                     while (RecoverySkill[i] == 0)
                     {
                         i = GetRand(Rcnt);
                     }
                     
                     loghistory.insert(loghistory.begin(), enemy[numbers[c.getOrder()][0]].GetName() + "��" + commandpara[c.getOrder()][RecoverySkill[i]].c_str() + "�I");
                     Manager.setEnemyFlg(true);
                     Manager.setAtk(c.getAtk(), atkpara[c.getOrder()][RecoverySkill[i]], elementpara[c.getOrder()][RecoverySkill[i]], skillpara[c.getOrder()][RecoverySkill[i]]);//�U���́Askill�̈З́A�Z����

                     c.setEnemyFeeling(35);

                 }



                     /*
                     int Rand = GetRand(99);

                     int cnt = 0;
                     if (Rand < 30)//�ʏ�U��
                     {
                         //���O
                         loghistory.insert(loghistory.begin(), enemy[numbers[c.getOrder()][0]].GetName() + "�̂��������I");
                         Manager.setEnemyFlg(true);
                         Manager.setAtk(c.getAtk(), 0, 0, 0);//�U���́Askill�̈З́A�Z����
                     }
                     else//�X�L���ōU���A�Ȃ��Ȃ�ʏ�U��
                     {
                         

                         Rand = GetRand(3);
                         while ((commandpara[c.getOrder()][Rand] == "�Ȃ�") && cnt < 100)
                         {
                             cnt++;
                             Rand = GetRand(3);
                         }
                         if (cnt < 50)//�X�L����p�����U��
                         {
                             loghistory.insert(loghistory.begin(), enemy[numbers[c.getOrder()][0]].GetName() + "��" + commandpara[c.getOrder()][Rand].c_str() + "�I");

                             Manager.setEnemyFlg(true);
                             Manager.setAtk(c.getAtk(), atkpara[c.getOrder()][Rand], elementpara[c.getOrder()][Rand], skillpara[c.getOrder()][Rand]);//�U���́Askill�̈З́A�Z����

                         }
                         else//�G���Z�������Ă��Ȃ��ꍇ���A����ς�ʏ�U��
                         {
                             loghistory.insert(loghistory.begin(), enemy[numbers[c.getOrder()][0]].GetName() + "�̂��������I");
                             Manager.setEnemyFlg(true);
                             Manager.setAtk(c.getAtk(), 0, 0, 0);//�U���́Askill�̈З́A�Z����
                         }


                         
                     }*/

                 }
             }
         }
     }




    /*�Q�[���I�����ɏI��点����.
    // �ǂݍ��񂾉摜�������̊J��
    chara::UnloadImage();
    */

}




