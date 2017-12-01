clear
echo "     __             __                        "
echo " .--|  .-----.-----|  .-----.--.--.-----.----."
echo " |  _  |  -__|  _  |  |  _  |  |  |  -__|   _|"
echo " |_____|_____|   __|__|_____|___  |_____|__|  "
echo "             |__|           |_____|           "
echo "               Desplegando worker"
echo " "

# Descargar biblioteca

compilar(){
    make
}

# Generar carpetas
generar_carpetas(){
	mkdir scripts
    mkdir tmp
}


#
# Script principal
#

# Opcion: descargar el ejecutable
echo "Desea compilar?"
select SN in "Si" "No"; do
    case $SN in
        Si ) compilar; break;;
        No ) break;;
    esac
done

# Opcion: generar carpetas
echo "Desea generar la estructura de carpetas?"
select SN in "Si" "No"; do
    case $SN in
        Si ) generar_carpetas; break;;
        No ) break;;
    esac
done