
Программа ПиКАДОГЛЯД.
=====================

Вступление.
-----------

Данная программа представляет собой простой просмотрщик, распечатчик и конвертер некоторых типов файлов,
порождаемых в процессе работы с системами радиотехнических САПР PCAD для операционной системы DOS.

Все такие САПР, получившие известное распространение в известных кругах, можно поделить на две группы:

*   PCAD 3-4 - главным образом используется последняя PCAD этой линейки - PCAD 4.5.
    Все они будут обозначаться в этом документе как PCAD4.
*   PCAD 7-8 - аналогично, применяется система, завершившая развитие это системы - PCAD 8.7.
    Для этих САПР будет применяться обозначение PCAD8.

Все данные САПР решают они и те же задачи, имеют одинаковые принципы и алгоритмы работы, весьма сходный 
пользовательский интерфейс и общее построение рабочего процесса.
Но, несмотря на это, два перечисленных выше семейства САПР всё же очень существенно отличаются как по
платформе реализации, так и по способу хранения внутренних данных, и, разумеется, по формату порождаемых
ими файлов.

Так, все программные модули PCAD4 являются настоящими программами реального режима DOS, хранят координаты
объектов в виде двухбайтовых слов, имеют квант коодинат, равный 0.1 мм для PC-CAPS и 0.01 мм для PC-CARDS.
Большая же часть модулей PCAD8 представляет собой программы защищённого режима, реализованные на платформе
DOS-расширителя Phar Lap, хранят координаты в виде четырёхбайтовых двойных слов, имеют квант коодинат,
равный 0.001 мм для PC-CAPS и 0.0001 мм для PC-CARDS (в сто раз меньше по сравнению с PCAD4, что сопровождается
пропорциональным увеличением точности).

Это различие отразилось даже в компоновке комплексов: так, во составе PCAD4 функция размещения радиоэлементов
на плате выделена в отдельный программный модуль PC-PLACE. Всякий, кто работает с PCAD4, наверное, задавал себе
вопрос: зачем же это сделано? Ведь оба редактора - PC-PLACE И PC-CARDS - работают непосредственно с конструктивами
элементов и платы, в её физических координатах, выполняют одну и ту же функцию - непосредственное проектирование
печатной платы. Так зачем же их разделять? Ведь это создаёт большие сложности в работе, так как операции трассировки
проводников и размещения элементов часто нужно выполнять итеративно и многократно. И даже отдельная программа
PC-PLACE имеет странные ограничения в работе - очень ограниченное количество узлов в сетке при использовании
функции авторазмещения компонентов.

Ответ здесь заключается именно в ограниченности доступной оперативной памяти для программ реального режима. 640к,
по мнению Билла Гейтса, как известно, должно было хватить на всё. И эти 640к разделяют между собой как исполняемый
код, так и обрабатываемые им данные. Но, как оказалось, с этим пророчеством он всё же несколько поторопился.
И если данные всё же можно перенести в расширенную память, то код никуда больше не денешь. Для сложных комплексов
640к всё же не хватает, даже при использовании EMS/XMS. Поэтому и пришлось прибегать как такому искусственному
приёму, как отделение PC-PLACE в отдельный программный модуль.

Программе же защищённого режима доступно неизмеримо больше памяти, поэтому надобность в PC-PLACE отпала и функция
размещения деталей вернулась в состав модуля PC-CARDS.

Лицензия программы.
-------------------

Данная программа является свободной и распространяется под лицензией GNU GPL 3 или более поздней.
Приложенная к ней документация распространяется в соответствии с условиями лицензии GNU FDL 1.3 или более поздней.

Системные требования.
---------------------

1. C++17 (STL)
2. GCC 11.2.0 (проверена работоспособность для Linux Mint 20.03 с установленными
пакетами gcc и MinGW-w64 9.0.0 Windows7_x86) или Visual Studio 2019+.

Стек технологий.
----------------

1. CMake 3.18.0
2. wxWidgets 2.8.12
3. CodeBlocks 20.03

Компиляция и сборка проекта.
----------------------------

Программа теоретически может быть скомпилирована любым компилятором C++17. Для облегчения этой операции к проекту приложен
сборочный cmake-скрипт. Кроме того, среди файлов репозитория присутствует уже подготовленный проект для среды разработки CodeBlocks.

Следует упомянуть, что интерфейс программы построен на основе оконного каркаса wxWidgets, и, следовательно,  требуется его наличие
на машине, на которой будет выполняться сборка. В состав файлов репозитория помимо cmake-скрипта и проекта для CodeBlocks входит
также и конфигурационный файл для интерактивного конструктора интерфейсов wxWidgets - wxSmith.

Функции программы.
------------------

Функции программы просты и незатейливы: открытие файлов PCAD, просмотр их в окне программы, печать (получение
твёрдой копии) изображения и преобразование его в ряд других, общеприменимых и более широко распространённых
графических форматов. Для эффективного решения этих задач программа предоставляет ряд инструментов. Эти инструменты,
способы их настройки и применения описаны в соответствующих разделах справочной системы программы, вызываемой из
основного окна по нажатию клавиши F1.

Все форматы PCAD-изображений являются векторными и состоят из ограниченного набора графических примитивов.
Спецификации поддерживаемых программой файлов содержатся в каталоге docs основного репозитория программы.

Ограничения программы и планы на будущее.
-----------------------------------------

Все существующие вокруг PCAD форматы можно разделить на "внутренние" и "внешние".  ВНутренние форматы - файлы,
предназначенные для передачи информации внутри комплекса PCAD, между составляющими его программами. Внешние
форматы - файлы, порождаемые в целях экспорта информации, передачи данных вовне системы, к какому-либо её
внешнему адресату (например, изготовителю разработанного в PCAD оборудования).

На данный момент программа поддерживает всего только четыре наиболее простых формата файлов PCAD: два из них
относятся к "внутренними", а два других - к "внешними". Из внутренних форматов пока реализована обработка
plot-файлов, создаваемый PCAD4 (файл PLT4), и plot-файлов, создаваемый PCAD8 (PLT8). Среди всех внутренних файлов
они наиболее просты по своей структуре и содержат только графическую информацию, минимально достаточную для
описания картинки. Никаких прочих дополнительных и метаданных такие файлы не содержат.

Поддерживаемые в настоящее время внешние файлы - GERBER-файлы, создаваемые программой PC-PHOTO, и EXCELLON-Файлы
сверловки, генерируемые PC-DRILL. В виде этой пары файлов информация обычно передаётся на завод-изготовитель
разработанных плат.

С поддержкой входных файлов на этом пока всё. Никакие другие файлы (а всего их несколько десятков) в настоящий
момент не обрарабатываются.

Функция экспорта в сторонние форматы тоже сейчас реализована лишь частично. Экспорт в DXF и WMF в данный момент
только декларирован (для него даже зарезервированы пункты меню), но реально не работает. При выборе соответствующих
пунктов в меню вы получите лишь сообщение об том, что такой функции в программе пока нет.

Кроме того, есть некоторые сложности с отображением текста (текстового примитива), входяящего в состав
PCAD-изображений некоторых форматов (в частности, содержащегося в plot-файлах). Для рисовки текста пока
используются встроенные функции wxWidgets, которые, в свою очередь, опираются на внутренний текстовый
движок из состава подлежащей графической библиотеки или операционной системы. Но разнообразие режимов вывода
текста, предусмотренного форматом файлов PCAD, требует создания специального текстового движка,
возможности которого должны быть шире, чем всё, что предоставляет модуль вывода текста каркаса wxWidgets.
Этот движок сейчас находится в процессе разработки и в состав программы не включён. Так что пока текстовые
примитивы, содержащиеся во входных файлах, рисуются не совсем точно и в ряде случаев будут выглядеть существенно
отличающимися от того, как они должны выглядеть на самом деле - в собственных редакторах комплекса PCAD.

В планах дальнейшего развития программы находятся завершение работы над отрисовкой текста, поддержка всех остальных
сколь-нибудь графических файлов, создаваемых САПР PCAD для DOS (а, возможно, и не только графических), и добавление
дополнительных инструментов просмотра и обработки изображений, если в таковых выявится потребность.

Возможно, целесообразным будет добавление и такой функциональности, которая находится уже за пределами, собственно,
просмотрщика. В частности, очень полезным было бы включение в программу таких функций, как непосредственная
генерация файлов GERBER и EXCELLON на основе plot- и pcb-данных, что позволило бы порождать и проверять
соответствующую технологическую информацию более гибко и оперативно.

Ну и, наконец, можно расширить функционал программы и ещё более сложными возможностями, которые уже превратят её
саму из просмотрщика-конвертера в некое слабое подобие САПР. Начинать здесь, очевидно, следует с собственного
автономного трассировщика печатных плат (альтернатива PC-ROUTE). Он мог бы получать входную информацию из файлов
типа pcb (или соответствующих им pdif) и на их основе разводить некоторые простые или специфические типы печатных
плат, которые иногда бывает сложно трассировать в самом PC-ROUTE, который, как известно, вовсе не идеален (хотя и
очень мощен и совсем не плох).

Так что планов - громадьё. Всё определяется только наличием времени и желания. Будем надеяться на лучшее, ждите...

Немного об авторе.
------------------

Разработчика этого опуса зовут Федотов Евгений. Я довольно опытный пользователь PCAD для DOS с большим стажем,
что и натолкнуло меня на мысль создания этой программы.

Контакты, посредством которых со мной можно связаться:

* Основной репозиторий : http://www.github.com/FedotovEv/PicadoGlance
* Электронная почта:
    + Основная : fedotov_ev@rambler.ru
    + Дополнительная: dichrograph@rambler.ru
* Сайт: http://www.dichrograph.ru

Буду чрезвычйно рад, если этот плод моего труда кому-то пригодится. По всем вопросам и предложениям - обращайтесь
по электронной почте. Если (внезапно) появятся какие-либо патчи, которые вы сочтёте целесообразным наложить на
программу, можно воспользоваться и встроенным механизмом гитхаба - "pull request".

Обращайтесь, но имейте в виду - общаюсь, как и работаю над свободными программами, я только в свободное время,
поэтому ответы на все обращения не гарантирую. Даже более того - скорее всего, ответов не будет. Но прочитать
и обдумать постараюсь всё.

С наилучшими пожеланиями, разработчик.
