# cnvToPdf v0.1c

���� jpg �摜��pdf��


## �@�\

- ���� jpg �摜��pdf��.
- �E�Ԃ��w��\
- ���J���w��\
- �^�C�g���ƒ��Җ��̎w��\

## �g����

usage > jpg2pdf.exe [�I�v�V����] jpg�t�@�C��...  
  1���ȏ�̎w�肳�ꂽjpg�摜���t�@�C�����Ń\�[�g���āA�P��pdf�t�@�C���𐶐�.  
  �\�[�g�̓t�@�C�����̐��l�����͐��l���m�Ŕ�r�i�Ⴆ�� img_2.jpg, img_10.jpg ���ɂȂ�j

�I�v�V����:
  -o  --output"FILE.pdf"               �o�͂���pdf�t�@�C����  
                                       �w�薳�Ȃ�ŏ���jpg�̃f�B���N�g������-T�ō̗p  
  -d  --out-dir"DIR"                   --out�w��̂Ȃ��ꍇ�̏o�̓f�B���N�g����  
      --digit[-]                       �t�@�C�����̐�����𐔒l�Ƃ��Ĕ�r(*)  [- ���Ȃ�]  
  -r  --r2l                            �E�Ԃ��w��  
  -r- --l2r                            ���Ԃ��w��(*)  
      --outline                        �A�E�g���C��(�Ŕԍ��ꗗ)��t��  
  -t  --title"�^�C�g��"                �^�C�g���w��  
  -a  --author"����"        ���Ҏw��  
  -T  --author-title"[����] �^�C�g��"  ���҂�[]�ň͂�Œ��҂ƃ^�C�g�����w��  
  -T  --author-title"���� - �^�C�g��"  ' - '������Œ��҂ƃ^�C�g�����w��  
  -mN                                  �ł̊J�����w��(0�`5).���L6�ɓ���  
      --page-layout-single             0:�P��(*)  
      --page-layout-one-column         1:�P�ŃX�N���[��  
      --page-layout-two-column-left    2:���J���X�N���[�����ŊJ�n  
      --page-layout-two-column-right   3:���J���X�N���[���ЕŊJ�n  
      --page-layout-two-page-left      4:���J�����ŊJ�n  
      --page-layout-two-page-right     5:���J���ЕŊJ�n  

  �� (*)�t�̓f�t�H���g�Ŏw���  
  �� �I�v�V�����ƃI�v�V���������̊Ԃɋ󔒂͓�����Ȃ��B-dHOGE �̂悤�ɋ󔒖��Ŏw��̂���.  
  

## ���Җ��A�^�C�g���� �ɂ���

���Җ��A�^�C�g���� �w��� pdf ���ɐݒ肷�邪�o�̓t�@�C�����ɂ��p����B(-o�w�薳��)  

���Ƃ��ƌl�I�ɂ� "���Җ� - �^�C�g����" �Ƃ����t�@�C�����̕t���������Ă����̂����A
iPad,iPhone�ł悭�g���Ă� i����(S,HD) ���Apdf���̏��͌����Ƀt�@�C������  
  "[���Җ�] �^�C�g����" (�� "�^�C�g����[���Җ�]")  
�̂悤�ȏꍇ�A[]���𒘎Җ��A����ȊO���^�C�g�����Ƃ��Ĉ����Ă���悤�Ȃ̂�
����ɕ킢�A�擪�� [���Җ�] ������΂���𒘎Җ��Ƃ����B
(���[]�͌l�I�ɒ��Җ��ȊO�Ŏg�����Ƃ�����̂Ŗ��T�|�[�g)

-o�w�薳�� -a -t �Ŏw�肵���ꍇ�̏o�̓t�@�C������ "[���Җ�] �^�C�g����" �ƂȂ�B
-T �̏ꍇ�́A-T �̎w��̂܂܂��t�@�C�����ƂȂ�B

�� �^�C�g���⒘�Җ��̕����́ASJIS�͈͂Ɍ�����. UNICODE���Ή�.


## �\�[�X�R���p�C��

�R���p�C������ɂ́A���̃\�[�X�̑��� libharu, zlib, libpng �̃\�[�X���K�v.  
�����̃\�[�X���_�E�����[�h���Ă��ă\�[�X����  
    libs_vc/  
�t�H���_����  
  libharu-RELEASE_2_3_0/  
  lpng1621/  
  zlib-1.2.8/  
�̂悤�ɔz�u�B(��L�͎��ۂɎg�p�����o�[�W�����B�����O��̃o�[�W�����ł����v�Ȃ͂�)  

libs_vc/bld_lib_bat/libs_config.bat ���e�L�X�g�G�f�B�^�ŕҏW�A  
  set CcName=vc90  
�̂悤�Ɏg�p����vc�̃o�[�W�������w��B(vc80,vc90,vc100,vc120,vc140) 
libs_vc/bld_lib_bat �t�H���_���J�����g�ɂ���  
  bld.bat  
�����s�Bmisc_inc, misc_lib �Ƀw�b�_��lib�������B  

�g�p����vc�ɍ��킹�� �����ꂩ�� lib_vc???/ �� sln ���J���ăR���p�C���B  
�Y���O�̃o�[�W�����Ȃ�K���ɃR���o�[�g�̂��ƁB  

��{�I��static�����^�C����x64�ݒ肠��B������ vc8exp �� express�ł��l������
dll�����^�C���g�p��x64���ɂ��Ă���B  
express�ł̏ꍇ�� bld.bat ���s�O�� libs_config.bat ��  
  set CcHasX64=0  
  set CcNoRtStatic=1  
�ɏ���������K�v����B  
(�����ۂ�express�łŃR���p�C�������킯�łȂ��̂Œ���.�_���Ȃ�S����)  

�� VC�̃o�[�W���������ɁAVisual Studio �̃}�N���ϐ� PlatformToolsetVersion ��
�p���Ă���B����� vc10(2010)�ȍ~�ɂ͂��邪�Avc9(2008)�ȑO�ɂ͖����̂�
�����ł� macro.vsprops �Ƃ����̂�ǉ����A���̒��� PlatformToolsetVersion ��
�ݒ肷��悤�ɂ��Ă���B

## ����
2016/02/28 v0.1  ���J
2016/02/29 v0.1b  libharu�̃r���h�Ŋm�F�����˂� demo ���r���h���Ă������A
���̒��� jpfont_demo.exe �̎��s�ŃG���[�I�����Ă��܂��Bjpfont_demo.c ���̃t�H���g��
MS-Mincyo �������ŁA�S��MS-Mincho�ɒu������Β��邪�A��ړI�łȂ��̂� demo ��
�r���h���Ȃ��悤�Ƀo�b�`�C��.
2016/08/25 v0.1c  �t�@�C������r���啶���������ʂɂȂ��Ă����̂𓯈ꎋ����悤�ɏC��
