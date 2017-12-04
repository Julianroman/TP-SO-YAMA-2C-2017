#
# COMMONS LIBRARY - SISOPUTNFRBA
#

# Clonar repo
git clone https://github.com/sisoputnfrba/so-commons-library.git ../so-commons-library

# Acceder a la carpeta
cd ../so-commons-library

# Compilar
make

# Instalar
echo "Ingrese password para instalar las commons..."
sudo make install

#
# READLINE
#

# Descargar e instalar READLINE
sudo apt-get install libreadline6 libreadline6-dev


