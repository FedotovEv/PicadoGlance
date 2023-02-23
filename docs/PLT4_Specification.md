
�������� (������������) ������� ������ PLT4.
============================================

�������� � ����� ��������.
--------------------------

����� PLT ��������� ������������ ����������� ��������� PCAD (PCCAPS � PCCARDS) � �������� ����������� �������������
������������ � ��� �����������, ���������� �� ����� �������� � ������������� ��������, ������� � �������� ����������
� ������ �������� ������� �������� ���� �������� (*.SYM, *.SCH, *.PRT, *.PCB). �������� � ������ ����������� ��������
��������� � ������� �� ������ ��������� ������� �������������� ����������.

��� �������� ����� �������� � ������ ����� ������� ������������ �������� ����� ����������� � ������������
����� �������� ��������.

��� �������������� ������� (���������� � �������) �������� � ���� ������ ������ ���� ������ - DBU.
����� �������� ������ DBU � ��������������� ��������� ����� ������� �� ���� ���������� ������� ���������.

��� ��������� PC-CAPS ��� ������ � ����������� ������� ������� ��������� ��������� (�������
��� ������ � ���������) ������������� ����������, ������� 10 DBU (1 DBU = 0.1 ��).
� ���������� ������� ������� ��������� - ����� ���� ����� = 0.01 ����� = 10 ����� = 0.254 ��
= 1 DBU (1 DBU = 0.01 ����� = 0.254 ��).

��� ���������� PC-CARDS � PC-PLACE ������� DBU �� ������� (� 10 ���) ������� (������). ��� ���
������ � ����������� ������� �������� ��������� ��������� ��-�������� �������� ���������, ��
�� ��� ������������� 100 DBU (1 DBU = 0.01 ��). � ���������� ������� �������� ��������� ���������
����� ��� �������� ���� ����� (���), � � ���� ������ ��� ����� ��������������� 1 DBU.
����� �������, ��� ���������� ������� � PC-CARDS �����: 1 DBU = 1 ��� = 0.001 ����� =
0.0254 ��.

������� ��������� � ��������� ����������� ���������, � ������������ ������������ ������������ ����: ��� �������
�������������, ���������� ����� �������, ��� ������� �����������, ���������� ����� �����.

��������� �������� ��������� � ������� ����� PLT4.
--------------------------------------------------

����� ��������� ����� PLT4, ������������ ����������� ��������� PCAD4, �������� ���:

1. ��������� �������������� �������
2. ������� ���������� ����
3. ������ ����������� ����������

����� ������ �� ��� ����� ������ ��������.

#### ��������� �������������� �������.

��������� (����������) ������������� - �������������, ������ �������� ���������� �� ����������� �����.

|��������    | ������     |    ���   |          ����������               |                 ��������                  |
|:----------:|:----------:|:--------:|:---------------------------------:|:-----------------------------------------:|
| 0          | 4 �����    | uint32_t |     ��������� ������� �����       |               =0x4E800400                 |
| 4          | 2 �����    | int16_t  | ���������� X ������ ������� ����  |                                           |
|            |            |          |    ���������� ��������������      |                                           |
| 6          | 2 �����    | int16_t  | ���������� y ������ ������� ����  |                                           |
|            |            |          |    ���������� ��������������      |                                           |
| 8          | 2 �����    | int16_t  | ���������� x ������� �������� ����|                                           |
|            |            |          |    ���������� ��������������      |                                           |
| 10         | 2 �����    | int16_t  | ���������� y ������� �������� ����|                                           |
|            |            |          |    ���������� ��������������      |                                           |
| 12         | 2 �����    | uint16_t |      ��� ������������ �������     | =0x2 ��� ���������� (��������) �������    |
|            |            |          |                                   | =0x7 ��� ����������� �������              |
| 14         | 2 �����    | uint16_t |   ��� ���������-��������� �����   | =0x1, ���� ���� ������ ���������� PC-CARDS|
|            |            |          |                                   |                ��� PC-PLACE               |
|            |            |          |                                   | =0xA, ���� ���� ������ ���������� PC-CAPS |
| 16         | 2 �����    | uint16_t | ���������� ������� � ������� ����| =NLAY                                     |
| 18         | 2 �����    | uint16_t |           ������� �����           | =0, ��������� ���������� ���������        |

#### ������� ���������� (������������) ����

����� ����� �� ���������� �������������� ������� ��������� ������� ���������� (������������) ����. � ���� �������
���������� ����� ������� �������, ������� ������� � ��������� � ���� "���������� ������� � ������� ����". ����� �����
������ - 8 ����.

##### ��������� ������� ����.

|��������    | ������     |     ���       |            ����������                |
|:----------:|:----------:|:-------------:|:------------------------------------:|
|  20        |  8 ����    | layer_struct  |  �������� ������� ����������� ����   |
| .......... | .......... | ............. | .................................... |
| .......... | .......... | ............. | .................................... |
| 20+N*8     |  8 ����    | layer_struct  | �������� ���������� ����������� ���� |

#### �������� layer_struct ������� ���� �������� ���:

|�������� �� ������ ������| ������     |    ���   |           ����������             | 
|:-----------------------:|:----------:|:--------:|:--------------------------------:|
| 0                       | 6 ����     | char[6]  |            ��� ����              |
| 6                       | 1 ����     | uint8_t  |���������� ����� ���� � ��������  |
|                         |            |          |������� ���� ��������� (�������  |
|                         |            |          |              VLYR)               |
| 7                       | 1 ����     | uint8_t  |������ ������� - ����, �����������|
|                         |            |          |���� (�� 0 �� 15)                 |

���� ��� ���� ������ 6 ����, �� ��� ����� ����������� �� ����� ���� ������ ������, �� ��� ��������������
����� � ���� ������ ����������� ���������� �� ����������� ������� ������.

�������� ����� ����� - ������������ ����������� ��������� �����������.
----------------------------------------------------------------------

����� ���������� ������� ���� ������� ������ �������� ����������� ����������, � ����������, ����������
�����������. ����� ���������� 9 �������������� ����������:

1. ���������� ����� ��������� ����������
2. ������� (�����)
3. �������������
4. ����������� (�����������) �������������
5. ����������
6. ���� ����������
7. ������ ������
8. ������� (��������, ��������)
9. ������������� (�������)

��������� ���������� ������� �������� ���� �� ������, ��� �����-���� ������������. ��� ����� ����� ���
������������� ������, ��� � ���������� �����. ��������� ����������� �� ���������� ������� �����.

### �������� �������������� ���������� �����������.

#### ���������� ����� ��������� ����������

����� ��������� - 4 �����.

|�������� �� ������ ������| ������     |    ���   |            ����������                 |          ��������          |
|:-----------------------:|:----------:|:--------:|:-------------------------------------:|:--------------------------:|
| 0                       | 2 �����    | uint16_t | ��������� ��������� "���������� �����"|          =0x80A9           |
| 2                       | 1 ����     | uint8_t  | ������� ����                          |  =0                        |
| 3                       | 1 ����     | uint8_t  | ������ ������� - ����, ������� �����  |     �������� �� 0 �� 15    |
|                         |            |          | ���������� ��������� ��������� (����� |                            |
|                         |            |          | ��������� "����������� �������������")|                            |

#### ������� (�����)

����� ��������� - 12 ������.

|�������� �� ������ ������| ������     |    ���   |            ����������                 |          ��������          |
|:-----------------------:|:----------:|:--------:|:-------------------------------------:|:--------------------------:|
| 0                       | 2 �����    | uint16_t | ��������� ��������� "����� (�������)" |          =0x80A8           |
| 2                       | 1 ����     | uint8_t  |             ��� �����                 | =0 - �������� ����� (SOLID)|           
|                         |            |          |                                       | =1 - ��������� (DOTTED)    |
|                         |            |          |                                       | =2 - ���������� (DASHED)   |
| 3                       | 1 ����     | uint8_t  |        ������ (�������) �����         |                            |
| 4                       | 2 �����    | int16_t  | ���������� X ��������� ����� �������  |                            |
| 6                       | 2 �����    | int16_t  | ���������� y ��������� ����� �������  |                            |
| 8                       | 2 �����    | int16_t  | ���������� x �������� ����� �������   |                            |
| 10                      | 2 �����    | int16_t  | ���������� y �������� ����� �������   |                            |

#### �������������

����� ��������� - 12 ������.

|�������� �� ������ ������| ������     |    ���   |            ����������                 |          ��������          |
|:-----------------------:|:----------:|:--------:|:-------------------------------------:|:--------------------------:|
| 0                       | 2 �����    | uint16_t | ��������� ��������� "�������������"   |          =0x80A7           |
| 2                       | 1 ����     | uint8_t  |             ��� �����                 | =0 - �������� ����� (SOLID)|           
|                         |            |          |                                       | =1 - ��������� (DOTTED)    |
|                         |            |          |                                       | =2 - ���������� (DASHED)   |
| 3                       | 1 ����     | uint8_t  |        ������ (�������) �����         |                            |
| 4                       | 2 �����    | int16_t  |    ���������� X ������ ����� �����    |                            |
|                         |            |          |            ��������������             |                            |
| 6                       | 2 �����    | int16_t  |    ���������� y ������ ����� �����    |                            |
|                         |            |          |            ��������������             |                            |
| 8                       | 2 �����    | int16_t  |    ���������� x ������� ������ �����  |                            |
|                         |            |          |            ��������������             |                            |
| 10                      | 2 �����    | int16_t  |    ���������� y ������� ������ �����  |                            |
|                         |            |          |            ��������������             |                            |

#### ����������� (�����������) �������������

����� ��������� - 12 ������.

|�������� �� ������ ������| ������     |    ���   |            ����������                 |          ��������          |
|:-----------------------:|:----------:|:--------:|:-------------------------------------:|:--------------------------:|
| 0                       | 2 �����    | uint16_t |   ��������� ��������� "�����������    |          =0x80A6           |
|                         |            |          |      (�����������) �������������"     |                            |
| 2                       | 1 ����     | uint8_t  | ���� �������� ������ - ������ ������� |    �������� �� 0 �� 15     |
| 3                       | 1 ����     | uint8_t  |            ������� ����               |    =0                      |           
| 4                       | 2 �����    | int16_t  |    ���������� X ������ ����� �����    |                            |
|                         |            |          |            ��������������             |                            |
| 6                       | 2 �����    | int16_t  |    ���������� y ������ ����� �����    |                            |
|                         |            |          |            ��������������             |                            |
| 8                       | 2 �����    | int16_t  |    ���������� x ������� ������ �����  |                            |
|                         |            |          |            ��������������             |                            |
| 10                      | 2 �����    | int16_t  |    ���������� y ������� ������ �����  |                            |
|                         |            |          |            ��������������             |                            |

#### ����������

����� ��������� - 16 ����.

|�������� �� ������ ������| ������     |    ���   |            ����������                 |          ��������          |
|:-----------------------:|:----------:|:--------:|:-------------------------------------:|:--------------------------:|
| 0                       | 2 �����    | uint16_t |    ��������� ��������� "����������"   |          =0x80A5           |
| 2                       | 1 ����     | uint8_t  |             ��� �����                 | =0 - �������� ����� (SOLID)|           
|                         |            |          |                                       | =1 - ��������� (DOTTED)    |
|                         |            |          |                                       | =2 - ���������� (DASHED)   |
| 3                       | 1 ����     | uint8_t  |        ������ (�������) �����         |                            |
| 4                       | 2 �����    | int16_t  |    ���������� X ������ ����������     |                            |
| 6                       | 2 �����    | int16_t  |    ���������� y ������ ����������     |                            |
| 8                       | 2 �����    | uint16_t |           ������ ����������           |                            |
| 10                      | 6 ������   | int8_t[6]|          ������� �����������          | =0                         |

#### ���� ����������

����� ��������� - 16 ����.

|�������� �� ������ ������| ������     |    ���   |            ����������                 |          ��������          |
|:-----------------------:|:----------:|:--------:|:-------------------------------------:|:--------------------------:|
| 0                       | 2 �����    | uint16_t | ��������� ��������� "���� ����������" |          =0x807A           |
| 2                       | 1 ����     | uint8_t  |             ��� �����                 | =0 - �������� ����� (SOLID)|           
|                         |            |          |                                       | =1 - ��������� (DOTTED)    |
|                         |            |          |                                       | =2 - ���������� (DASHED)   |
| 3                       | 1 ����     | uint8_t  |        ������ (�������) �����         |                            |
| 4                       | 2 �����    | int16_t  |    ���������� x ������ ����������     |                            |
| 6                       | 2 �����    | int16_t  |    ���������� y ������ ����������     |                            |
| 8                       | 2 �����    | int16_t  |    ���������� x ��������� ����� ����  |                            |
| 10                      | 2 �����    | int16_t  |    ���������� y ��������� ����� ����  |                            |
| 12                      | 2 �����    | int16_t  |    ���������� x �������� ����� ����   |                            |
| 14                      | 2 �����    | int16_t  |    ���������� y �������� ����� ����   |                            |

��� �������� ����� ������ ������������ ����, �� ���� ������ ����� �� ��� �� �����, �� ������ (���������) ����� ������������
��� ������� ������� ���� (���������� �� ������ �� ���� �����) � ���������� ����, � ������ (��������) - ������ ��� �������
� ���������� ����.

#### ������ ������

����� ��������� - ����������.

|�������� �� ������ ������| ������   |    ���    |            ����������                 |           ��������         |
|:-----------------------:|:--------:|:---------:|:-------------------------------------:|:--------------------------:|
| 0                       | 2 �����  | uint16_t  |  ��������� ��������� "������ ������"  |           =0x80A2          |
| 2                       | 2 �����  | uint16_t  |         ������� �����������           |  =0                        |
| 4                       | 2 �����  | int16_t   |   ���������� x ����� �������� ������  |                            |
| 6                       | 2 �����  | int16_t   |   ���������� y ����� �������� ������  |                            |
| 8                       | 1 ����   | uint8_t   |          ���������� ������            | ����������� ����           |
| 9                       | 2 �����  | uint16_t  |             ������ ������             | ����������� ����           |
| 11                      | 1 ����   | uint8_t   |     ������������ (��������) ������    | ����������� ����           |
| 12                      | N ������ | char[N]|  |      ���������� ��������� ������      | ������ ����������� ����    |

* #### ���������� (�����������) ������

|�������� �������|���������� ������                   |
|:--------------:|:----------------------------------:|
|       0x0      | ����������� 0 ��������, �����      |
|       0x4      | ����������� 0 ��������, ���������  |
|       0x1      | ����������� 90 ��������, �����     |
|       0x5      | ����������� 90 ��������, ��������� |
|       0x2      | ����������� 180 ��������, �����    |
|       0x6      | ����������� 180 ��������, ���������|
|       0x3      | ����������� 270 ��������, �����    |
|       0x7      | ����������� 270 ��������, ���������|

������� �� ��������� ���������� �������� �������� ������ ������� �������.
��� ����� ��������, ������� ������������ ������������ ������������� 2-� ����� ����.

��� ������������ ������ � ����������� ������� ���������� ����� ������� ����� �����,
������ �� ������� ��������� ������ �������� ������ �� �����������, � ������ ����� - �� ���������.

* #### ������������ (��������) ������

|�������� ���� ������������|      ������������ ������      |
|:------------------------:|:-----------------------------:|
|          0x21            |   ������������ "����-����"    |
|          0x23            |   ������������ "����-�����"   |
|          0x22            |   ������������ "����-���"     |
|          0x19            |   ������������ "�����-����"   |
|          0x1B            |   ������������ "�����-�����"  |
|          0x1A            |   ������������ "�����-���"    |
|          0x29            |   ������������ "�����-����"   |
|          0x2B            |   ������������ "�����-�����"  |
|          0x2A            |   ������������ "�����-���"    |

����� ������������ ����� ASCIIZ-������, �� ���� ������ ������������ ������� ������. ����� ����, ����� ����������
������ (� ������ ������������ ����) ������ ���� ������ 4 ������ (�������� �����). ���� ��� �� ���, ���
����������� ������ �� ������� �������� ����� (�� ���� 4 ����).

���� "������ ������" = WF ���������� ������ ������, �� ����� ����� ��������������� ��� 7-������� � �������� ������
������ �������������� ���: 

W = HIBYTE(WF) * 128 + LOBYTE(WF) = <������� ����>(WF) * 128 + <������� ����>(WF)

#### ������� (��������, ��������)

����� ��������� - 8 ����.

|�������� �� ������ ������| ������     |    ���   |            ����������                 |          ��������          |
|:-----------------------:|:----------:|:--------:|:-------------------------------------:|:--------------------------:|
| 0                       | 2 �����    | uint16_t |��������� ��������� "�������(��������)"|          =0x8095           |
| 2                       | 1 ����     | uint8_t  |����� �������� �� ������� ����� �������|                            |
| 3                       | 1 ����     | uint8_t  |          ���������� �������           |  =0 - ������� 0 ��������   |
|                         |            |          |                                       |  =1 - ������� 90 ��������  |
|                         |            |          |                                       |  =2 - ������� 180 �������� |
|                         |            |          |                                       |  =3 - ������� 270 �������� |
| 4                       | 2 �����    | int16_t  |    ���������� x ������ �������        |                            |
| 6                       | 2 �����    | int16_t  |    ���������� y ������ �������        |                            |

#### ������������� (�������)

����� ��������� - ����������.

��� ��������� ��������, ��������� �� ������ �������� ���������� ��������� ��������, � ����� �������������
���������� �������� ���� ������ ��������� - ������ ������ ����� ��������, �������, � ���� �������, ����� ����
�������������� � ��������.

������� ������������� �������� ������ ��������, �������, � ���� �������, ������� �� ���������� ��������� �������������
��������� � ������ ������ ��������, ���������� ������� � ������� ���������� � ����� ���������� ������ � ��������.
����� - ������ ������ �� ��������, ������� ��������� ������ ��� ����.

|�������� �� ������ ������| ������     |     ���    |            ����������                 |          ��������           |
|:-----------------------:|:----------:|:----------:|:-------------------------------------:|:---------------------------:|
| 0                       | 2 �����    |  uint16_t  |  ��������� ��������� "�������������"  |          =0x80A3            |
| 2                       | 1 ����     |  uint8_t   |��� ���������� ����������� ������������|  =1 - �������� �������      |
|                         |            |            |           ��������������              |  =2 - ���������             |
| 3                       | 1 ����     |  uint8_t   |     ������ (�������) �������� ���     |                             |
|                         |            |            |       ��������� ������ ��������       |                             |
| 4                       | 4 �����    |VertexStruct|    ��������� ������� ��������������   |  ������ ������� ��������    |
|........................ |............|............|.......................................|.............................|
| xxx                     | 4 �����    |VertexStruct|    ��������� ������� ��������������   | ��������� ������� ��������  |
|........................ |............|............|.......................................|.............................|
| xxx                     | 4 �����    |VertexStruct|    ��������� ������� ��������������   | ��������� ������� ��������  |
| xxx                     | xxx        | xxx        |        ������� ������ ������          |  ������ ������ �� ��������  |
|........................ |............|............|.......................................|.............................|
| xxx                     | xxx        | xxx        |    ����������� ������� ������ ������  |��������� ����� ������ ������|

#### VertextStruct:

��������� VertextStruct ����� ������� ���

    struct VertextStruct
    {
      int16_t vertex_x; // ���������� x ������� ��������
      int16_t vertex_y; // ���������� y ������� ��������
    };

��� ���:

|�������� �� ������ ������| ������     |     ���    |         ����������               |
|:-----------------------:|:----------:|:----------:|:--------------------------------:|
| 0                       | 2 �����    |  int16_t   |  ���������� x ������� ��������   |
| 2                       | 2 �����    |  int16_t   |  ���������� y ������� ��������   |

��������� ������� ������ ����������� �� ������� �� ����� ��������� ��������� VertexStruct �����
�� ��� 4-�������� �������� (0x0000807F, 0x0000807E ��� 0x00008080), ����� � ���������� �������
������� ����.

����� ����� �� ������� ������ �������������� � ����� ���������� �������� ������ � ��������. �����
��������, � ���� �������, ������������ ��������� ����� ������ ���������, ������ �� ������� ����������
��������� ������. �������� ������ ������������� ���� �� ������ ��������, ��� ������������.

��������� ���������� �������� ������ ������ ����� ����� ��� ������ �����.

* #### ����� ��� ������� �������:

|�������� �� ������ ������| ������     |     ���    |            ����������                 |               ��������             |
|:-----------------------:|:----------:|:----------:|:-------------------------------------:|:----------------------------------:|
| 0                       | 4 �����    |  uint32_t  |  ��������� ������� ������ �� �������� |  =0x0000807F - ������� ������      |
| 4                       | 2 �����    |  int16_t   |    ���������� x ������ ������� ������ |                                    |
| 6                       | 2 �����    |  int16_t   |    ���������� y ������ ������� ������ |                                    |
| 8                       | 2 �����    |  uint16_t  |         ������ ������� ������         |                                    |
| 10                      | 2 �����    |  uint16_t  |          ������� �����������          | =0                                 |

* #### ����� ��� ������������� �������:

|�������� �� ������ ������| ������     |     ���    |            ����������                 |               ��������             |
|:-----------------------:|:----------:|:----------:|:-------------------------------------:|:----------------------------------:|
| 0                       | 4 �����    |  uint32_t  |   ��������� ������������� ������ ��   | =0x0000807E - ������������� ������ |
|                         |            |            |              ��������                 |                                    |
| 4                       | 4 �����    |VertexStruct|��������� ������� ������������� ������ |         ������ ������� ������      |
|........................ |............|............|.......................................|....................................|
| xxx                     | 4 �����    |VertexStruct|��������� ������� ������������� ������ |        ��������� ������� ������    |
|........................ |............|............|.......................................|....................................|

��������� ������� ������ ����������� �� ������� �� ����� ��������� ��������� VertexStruct ����� �� ��� 
4-�������� �������� (0x0000807F, 0x0000807E ��� 0x00008080), � ������� ���������� ��������� ������� ������.

* #### ����� ��� ������������ �������� ������.

��� ����������� �������� �������� � ������� ������ �� ���������.

|�������� �� ������ ������| ������     |     ���    |              ����������               |               ��������             |
|:-----------------------:|:----------:|:----------:|:-------------------------------------:|:----------------------------------:|
| 0                       | 4 �����    |  uint32_t  |    ��������� ����� ������ ������      |=0x00008080 - ����� ������ ������ � |
|                         |            |            |              �� ��������              |�������� �������� � �����           |