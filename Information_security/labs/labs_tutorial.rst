========================================
Лабораторные работы по защите информации
========================================

1. Генерация ключа RSA
======================

  I. Открываем ``RSAbegin/MyRNDseedCreate.mws`` в Maple.

     #. Заменяем::

          SurName_N := "SurName_N";

        на фамилию и первую букву имени, например на ``"Rutsky_V"``:

          SurName_N := "Rutsky_V";

     #. Заменяем::

          My_Seed:= 123412345567678998012342435357789980;

        на произвольное, но другое число.

     #. Выполняем скрипт.

        Теперь в директории ``RSAbegin`` создан файл 
        ``RNDseed_Rutsky_V.byt`` ---
        в нём записано произвольное число, которое было указано ранее,
        в двоичном формате.

  II. Открываем ``RSAbegin/RSAkeyGen.mws`` в Maple.

     #. Заменяем::

          SurName_N := "SurName_N";
     
     #. Заменяем::
     
          TargetFileName:= "RSAnPK_SurName_N";
        
        на::

          TargetFileName:= "RSAnPK_"||SurName_N;

     #. Выполняем скрипт.

        Теперь в директории RSAbegin созданы следующие файлы:

        1) ``RSAfact_Rutsky_V.byt`` --- `[p1, p2, L(n)]` --- два простых 
           числа и значение `функции Кармайкла 
           <http://en.wikipedia.org/wiki/Carmichael_function>`_ 
           от `n = p1 * p2`,

        2) ``RSAnPK_Rutsky_V.byt`` --- `[n, ePK]` --- 
           сгенерированный открытый ключ,

        3) ``RSASK_Rutsky_V.byt`` --- `[SK]` --- сгенерированный закрытый 
           ключ, `SK = ePK^-1 mod phi(n)`.

  III. Открываем ``RSAbegin/SendMy_nPK_RSAECBencr.mws`` в Maple.

     #. Заменяем::

          SurName_N := "SurName_N";
     
     #. Создаём файл вида ``Rutsky_V_passw.txt``,
        в котором сохраним пароль,
        который был ранее передан С.В. Стахову.

     #. Выполняем скрипт.

        Теперь в директории ``RSAbegin`` создан файл  
        ``RSAnPK_Rutsky_V_ECB.byt`` содержащий в себе сгенерированный ранее
        открытый ключ 
        и пароль, защифрованный открытым ключом С.В. Стахова --- 
        его необходимо отправить С.В. Стахову.

.. vim: set ts=1 sw=1 et:
