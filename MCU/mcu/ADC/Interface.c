//Funktion aufrufen um die Werte des ADC in den Algorithmus zu laden
//Matrix Daten als Speicherschnittstelle auf die der ADC und der Algorithmus zugreifen können

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

#define windowsize

//Definition von ADC-main-Funktion
void ADC(uint32_t ADC_SAMPLE_BUF_SIZE);


int ADCout(int datenpushnr) //datenpushnr, an welcher Stelle Daten gelesen werden sollen
{
	//Matrix definiert, 3 Arrays aus denen Werte genommen werden können
	int daten[3][windowsize];
	//Definition Outputvariable
	uint32_t datenpush;
	//Daten vom ADC nehmen
	ADC(windowsize, &daten);
	//Daten abspeichern
	datenpush = daten[datenpushnr];
	
	return datenpush;
}