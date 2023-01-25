Ich habe die nicht von mir erstellten Dateien in der Ausgabe von CppCheck deaktiviert um die Übersicht zu verbessern.

Weiterhin bestehende Misra Verstöße:

---pong.c---
- 2.7 in den Task Funktionen, Ist notwendig um Compiler Warnings zu beheben, da FreeRTOS den Parameter bei allen Task Funktionen benötigt.
- 15.5 in main(), Ist notwendig, da Fehler beim Task erstellen zu Beenden führen soll.
- 11.4 einige, Es sind alles Zugriffe auf Hardwareregister und zu diesem Zweck darf der Verstoß laut der Misra Dokumentation auftretten.
(Misra Dokumentation: "Casting between a pointer and an integer type should be avoided where possible, but may be necessary when addressing memory mapped registers or other hardware specific features.")

---pong.h---
- 2.5 einige, Die Makros werden alle in der pong.c verwendet. Um den Misra Fehler zu beheben könnte man sie in die pong.c verschieben, aber mit dem Header ist es sauberer.

---pong_field.c---
keine

---pong_field.h---
- 8.7 add_score(), Muss öffentlich sein, da in pong.c verwendet.
- 8.7 write_field_to_framebuffer(), siehe oben
- 8.7 do_move(), siehe oben
- 8.7 move_paddle(), siehe oben
- 8.7 init_pong(), siehe oben
- 2.5 einige, Die Makros werden alle in der pong_field.c verwendet. Um den Misra Fehler zu beheben könnte man sie in die pong_field.c verschieben, aber mit dem Header ist es sauberer.
