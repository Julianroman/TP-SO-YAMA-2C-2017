clear
echo "     __             __                        "
echo " .--|  .-----.-----|  .-----.--.--.-----.----."
echo " |  _  |  -__|  _  |  |  _  |  |  |  -__|   _|"
echo " |_____|_____|   __|__|_____|___  |_____|__|  "
echo "             |__|           |_____|           "
echo "               Desplegando nodo"
echo " "

WORKERPATH="Worker/Release"
DATANODEPATH="DataNode/Release"
WORKERCONFIG="worker-config.cfg"
DATANODECONFIG="nodo-config.cfg"


if [ "$1" = "-d" ]
then
	WORKERPATH="Worker/Debug"
	DATANODEPATH="DataNode/Debug"
fi

mkdir -p "$WORKERPATH"
mkdir -p "$DATANODEPATH"

# Descargar biblioteca

compilar(){
	make -C "$WORKERPATH"
	make -C "$DATANODEPATH"
}

# Escribir el archivo de configuracion
generar_configuracion(){
	echo "Ingrese la ip del FileSystem"
	echo -n "> "
	read IPFS

	echo "FS_IP=$IPFS" > "$WORKERPATH/$WORKERCONFIG"
	echo "IP_FILESYSTEM=$IPFS" > "$DATANODEPATH/$DATANODECONFIG"

	echo "FS_PUERTO=5040" >> "$WORKERPATH/$WORKERCONFIG"
	echo "PUERTO_FILESYSTEM=5040" >> "$DATANODEPATH/$DATANODECONFIG"

	echo "PUERTO_WORKER=5041" >> "$DATANODEPATH/$DATANODECONFIG"
	echo "PUERTO_DATANODE=5060" >> "$DATANODEPATH/$DATANODECONFIG"

	echo "Ingrese el numero de nodo"
	echo -n "> "
	read NRONODO

	echo "NUMERO_NODO=$NRONODO" >> "$DATANODEPATH/$DATANODECONFIG"

	echo "RUTA_DATABIN=../../data.bin" >> "$DATANODEPATH/$DATANODECONFIG"
	echo "RUTA_DATABIN=../../data.bin" >> "$WORKERPATH/$WORKERCONFIG"


	echo "Ingrese la cantidad de bloques"
	echo -n "> "
	read CANTBLOQUES

	echo "CANTIDAD_BLOQUES=$CANTBLOQUES" >> "$DATANODEPATH/$DATANODECONFIG"

	IPCANDIDATES=( $(hostname -I))	
	echo "Ingrese la ip del Datanode"
	echo "Posibles candidatos: $IPCANDIDATES" 
	echo -n "> "
	read IPDATANODE

	echo "IP_DATANODE=$IPDATANODE" >> "$DATANODEPATH/$DATANODECONFIG"
	
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


# Opcion: generar carpetas
echo "Desea generar los archivos de configuracion?"
select SN in "Si" "No"; do
    case $SN in
        Si ) generar_configuracion; break;;
        No ) break;;
    esac
done
