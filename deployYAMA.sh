clear
echo "     __             __                        "
echo " .--|  .-----.-----|  .-----.--.--.-----.----."
echo " |  _  |  -__|  _  |  |  _  |  |  |  -__|   _|"
echo " |_____|_____|   __|__|_____|___  |_____|__|  "
echo "             |__|           |_____|           "
echo "               Desplegando YAMA"
echo " "


YAMAPATH="YAMA/Release"
YAMACONFIG="yama-config.cfg"


if [ "$1" = "-d" ]
then
	YAMAPATH="YAMA/Debug"
fi

mkdir -p "$YAMAPATH"

# Descargar biblioteca

compilar(){
	make -C "$YAMAPATH"
}

# Escribir el archivo de configuracion
generar_configuracion(){
	echo "Ingrese la ip del FileSystem"
	echo -n "> "
	read IPFS
	
	echo "FS_IP=$IPFS" > "$YAMAPATH/$YAMACONFIG"
	echo "FS_PUERTO=5040" >> "$YAMAPATH/$YAMACONFIG"

	echo "Ingrese el retardo de planificacion (en ms)"
	echo -n "> "
	read RETARDO

	echo "RETARDO_PLANIFICACION=$RETARDO" >> "$YAMAPATH/$YAMACONFIG"

	echo "Elija el algoritmo:"
	select ALG in "Clock" "WeightedClock"; do
	    case $ALG in
	        Clock ) echo "ALGORITMO_BALANCEO=CLOCK" >> "$YAMAPATH/$YAMACONFIG"; break;;
	        WeightedClock ) echo "ALGORITMO_BALANCEO=WCLOCK" >> "$YAMAPATH/$YAMACONFIG";break;;
	    esac
	done

	IPCANDIDATES=( $(hostname -I))	
	echo "Ingrese la ip del YAMA"
	echo "Posibles candidatos: $IPCANDIDATES" 
	echo -n "> "
	read IPYAMA

	echo "YAMA_IP=$IPYAMA" >> "$YAMAPATH/$YAMACONFIG"
	echo "YAMA_PUERTO=5041" >> "$YAMAPATH/$YAMACONFIG"

	echo "Ingrese la carga base"
	echo -n "> "
	read CARGABASE

	echo "BASE=$CARGABASE" >> "$YAMAPATH/$YAMACONFIG"


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
