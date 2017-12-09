/*
 * responder_SOLICITUD.cinciarPlanificacion
 */

#include <utilidades/protocol/types.h>
#include <commons/collections/dictionary.h>
#include <stdio.h>
#include "responses.h"
#include "../YAMA.h"
#include "../Planificador.h"

void responder_SOLICITUD(int socket,void* data,HEADER_T tipoDeMensaje){
	if(tipoDeMensaje == SOLICITUD_JOB){
		res_SOLICITUD_JOB(socket, data);
	}
	else if(tipoDeMensaje == RESPUESTA_MASTER){
		res_RECIBIR_INFO(socket, data);
	}
}

void res_SOLICITUD_JOB(int socket,void*  data){
	payload_SOLICITUD_JOB* payload = data;
	log_trace(logYAMA,"SOLICITUD_JOB recibida. Empezando planificacion\n");
	int buscarJob(t_job_master* job_master){
		return job_master->master_socket == socket;
	}
	t_job_master* job_master = list_find(MastersJobs, (void*)buscarJob);
	//MANDAR A PLANIFICAR
	iniciarPlanificacion(payload->nombreArchivo, job_master);
}

void res_RECIBIR_INFO(int socket, void* data){
	payload_RESPUESTA_MASTER* infoMaster = data;
	int buscarJob(t_job_master* job_master){
		return job_master->master_socket == socket;
	}
	t_job_master* job_master = list_find(MastersJobs, (void*)buscarJob);
	if(job_master->master_id == infoMaster->id_master){
		responderSolicitudMaster(infoMaster, job_master);
	}
	else{
		log_trace(logYAMA, "Hubo un error con la información recibida");
	}
}
