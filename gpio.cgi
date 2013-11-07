#!/bin/sh

case $QUERY_STRING in
	*38*) #frente
		/bin/echo "0" > /dev/gpio0
		/bin/echo "1" > /dev/gpio1
		/usr/bin/led 3 0
		;;
	*40*) #traz
		/bin/echo "1" > /dev/gpio0
		/bin/echo "0" > /dev/gpio1
		/usr/bin/led 3 0
		;;	
	*76*) #para
		/bin/echo "1" > /dev/gpio0
		/bin/echo "1" > /dev/gpio1
		/usr/bin/led 3 1
		;;
	*80*) #para
		/bin/echo "1" > /dev/gpio0
		/bin/echo "1" > /dev/gpio1
		/usr/bin/led 3 1
		;;
	*37*) #esquerda
		/bin/echo "1" > /dev/gpio2
		/bin/echo "0" > /dev/gpio3
		/usr/bin/led 2 0
		;;
	*39*) #direita
		/bin/echo "0" > /dev/gpio2
		/bin/echo "1" > /dev/gpio3
		/usr/bin/led 2 0
		;;
	*74*) #desvira
		/bin/echo "1" > /dev/gpio2
		/bin/echo "1" > /dev/gpio3
		/usr/bin/led 2 1
		;;
	*78*) #desvira
		/bin/echo "1" > /dev/gpio2
		/bin/echo "1" > /dev/gpio3
		/usr/bin/led 2 1
		;;
	*65*) #acende farol
		/bin/echo "0" > /dev/gpio4
		/bin/echo "0" > /dev/gpio5
		/usr/bin/led 0 1
		;;
	*66*) #apaga farol
		/bin/echo "1" > /dev/gpio4
		/bin/echo "1" > /dev/gpio5
		/usr/bin/led 0 0
		;;
esac

exit 0
