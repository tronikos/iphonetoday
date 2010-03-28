#pragma once

#include "CIcono.h"

class CEstado
{
public:
	CEstado(void);
	~CEstado(void);

	// Variables 
	POINTS posObjetivo; // Posicion Objetivo en una transicion
	UINT pantallaActiva;
	BOOL hayMovimiento;
	CIcono *iconoActivo;

	RECT cuadroLanzando;	// Posicion Objetivo en una transicion de lanzamiento
	UINT estadoCuadro;		// 0=Desactivado, 1=Lanzando, 2=Abierto
	LONG timeUltimoLanzamiento;
	BOOL debeCortarTimeOut;
};
