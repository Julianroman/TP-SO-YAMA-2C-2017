clear
echo "     __             __                        "
echo " .--|  .-----.-----|  .-----.--.--.-----.----."
echo " |  _  |  -__|  _  |  |  _  |  |  |  -__|   _|"
echo " |_____|_____|   __|__|_____|___  |_____|__|  "
echo "             |__|           |_____|           "
echo "               Desplegando master"
echo " "


MASTERPATH="Master/Release"
MASTERCONFIG="master-config.cfg"


if [ "$1" = "-d" ]
then
	MASTERPATH="Master/Debug"	
fi

mkdir -p "$MASTERPATH"

# Descargar biblioteca

compilar(){
	make -C "$MASTERPATH"
}

# Escribir el archivo de configuracion
generar_configuracion(){
	echo "Ingrese la ip del YAMA"
	echo -n "> "
	read IPYAMA
	echo "Ingrese el limite de balanceo"
	echo -n "> "
	read BALANCEO
	
	echo "YAMA_IP=$IPYAMA" > "$MASTERPATH/$MASTERCONFIG"
	echo "YAMA_PUERTO=5041" >> "$MASTERPATH/$MASTERCONFIG"
	echo "BALANCEO=$BALANCEO" >> "$MASTERPATH/$MASTERCONFIG"
}

#
# Script principal
#

# Opcion: descargar el ejecutable
if [ "$1" != "-d" ]
then

	echo "Desea compilar?"
	select SN in "Si" "No"; do
	    case $SN in
	        Si ) compilar; break;;
	        No ) break;;
	    esac
	done

fi

# Opcion: generar el archivo de configuracion
echo "Desea generar el archivo de configuracion?"
select SN in "Si" "No"; do
    case $SN in
        Si ) generar_configuracion; break;;
        No ) break;;
    esac
done
