#include "listas.h"

void* find_cache_element_given_ID(void* ID_encontrar, uint32_t* bytes, t_log* logger, queue_code queue_cod){
	//TODO armar esta funcion (devuelva void* message_data) con mutex_cache (duplicar el message_data de la cache)
	void* message_data;
	t_partition* partition;

	bool _soy_ID_buscado(void* p){
		return ((t_partition*) p)->available == false && ((t_partition*) p)->ID_message == (uint32_t) ID_encontrar;
	}
	pthread_mutex_lock(&mutex_cache);
	partition = list_find(partitions, _soy_ID_buscado);
	if(partition != NULL){
		*bytes = partition->size;
		message_data = malloc(*bytes);
		memcpy(message_data, partition->initial_position, *bytes);
		update_LRU(partition);
	}
	pthread_mutex_unlock(&mutex_cache);

	if(partition == NULL){
		message_data = NULL;
		log_warning(logger, "El mensaje ya no se encuentra en la cache");
		//borrar el mensaje de la cola en este punto para que no haya loop infinito
		t_list* element_to_delete = list_create();
		list_add(element_to_delete, ID_encontrar);
		list_add(element_to_delete, (void*)queue_cod);
		delete_messages_from_queue(element_to_delete);
	}

	return message_data;
}

t_pending* remove_element_given_ID_short(uint32_t ID_encontrar, t_list* cola){
	bool _soy_ID_buscado(void* p){
		return ((t_pending*) p)->ID_mensaje == ID_encontrar;
	}
	return list_remove_by_condition(cola, _soy_ID_buscado);
}

t_pending* remove_ID_short(uint32_t ID_encontrar, t_list* colaIDs){
	bool _soy_ID_buscado(void* p){
		return ((uint32_t) p) == ID_encontrar;
	}
	return list_remove_by_condition(colaIDs, _soy_ID_buscado);
}

t_pending* find_element_given_ID_short(void* ID_encontrar, t_list* colaIDs){
	bool _soy_ID_buscado(void* p){
		return ((uint32_t) p) == (uint32_t) ID_encontrar;
	}
	return list_find(colaIDs, _soy_ID_buscado);
}

t_pending* find_element_given_ID(void* ID_encontrar, t_list* cola, pthread_mutex_t* mutex_cola, uint32_t* bytes, uint32_t* id_co, void** datos_mensaje, t_log* logsub){
	t_pending* elemento;
	bool Default = strcmp(memory_algorithm, "DEFAULT") == 0;

	bool _soy_ID_buscado(void* p){
		return ((t_pending*) p)->ID_mensaje == (uint32_t) ID_encontrar;
	}

	pthread_mutex_lock(mutex_cola);
		elemento = list_find(cola, _soy_ID_buscado);
		if(elemento != NULL){
			*bytes = elemento->bytes;
			if(Default){
			*datos_mensaje = malloc(*bytes);
			memcpy(*datos_mensaje, elemento->datos_mensaje, *bytes);
			}
			if(elemento->ID_correlativo != 0)
				*id_co = elemento->ID_correlativo;
		}
	pthread_mutex_unlock(mutex_cola);


	if(elemento == NULL)
		log_debug(logsub, "No existe mas el mensaje en la cola\n");

	return elemento;
}

t_suscriber* find_suscriber_given_ID(void* ID_encontrar, t_list* subs, pthread_mutex_t* mutex_subs){
	t_suscriber* suscriber;

	bool _soy_ID_buscado(void* p){
		return ((t_suscriber*) p)->ID_suscriber == (uint32_t) ID_encontrar;
	}

	pthread_mutex_lock(mutex_subs);
		suscriber = list_find(subs, _soy_ID_buscado);
	pthread_mutex_unlock(mutex_subs);

	if(suscriber == NULL)
		log_info(logger, "No se encontro el suscriptor %d en la lista", (int) ID_encontrar);
	else
		log_info(logger, "Si se encontro al suscriptor %d en la lista", (int) ID_encontrar);

	return suscriber;
}

void dump_cache(void){

	char* dump = string_new();
	string_append(&dump, "\n-----------------------------------------------------------------------------------------------------------------------------\n");
	string_append(&dump, "Dump: ");
	char* time = temporal_get_string_time("%d/%m/%y %H:%M:%S");
	string_append(&dump, time);
	free(time);

	uint32_t list_amount;
	uint32_t i;

	list_amount = list_size(partitions);
	for(i=1; i<(list_amount + 1); i++){
		t_partition* partition = list_get(partitions, (i-1));

		string_append(&dump, "\nParticion ");
		char* num = string_itoa(i);
		string_append(&dump, num);
		free(num);
		string_append(&dump, ": [");
		char* initial = string_from_format("%d", partition->initial_position - mem);
		char* final = string_from_format("%d", partition->final_position - mem);
		string_append(&dump, initial);
		string_append(&dump, " - ");
		string_append(&dump, final);
		free(initial);
		free(final);

		if(partition->available){
			string_append(&dump, "].\t[L]");
			string_append(&dump, "\tSize: ");
			char* size = string_itoa(partition->size);
			string_append(&dump, size);
			free(size);
			string_append(&dump, "b");

		}
		else{
			uint32_t partition_size = (partition->final_position - mem) - (partition->initial_position - mem);
			uint32_t data_size = partition->size;
			string_append(&dump, "].\t[X]");
			string_append(&dump, "\tSize: ");
			char* size = string_itoa(partition_size);
			string_append(&dump, size);
			free(size);
			string_append(&dump, "b");
			if(data_size < partition_size){
				string_append(&dump, " (");
				char* data_size_char = string_itoa(data_size);
				string_append(&dump, data_size_char);
				free(data_size_char);
				string_append(&dump, "b)");
			}
			if(strcmp(replacement_algorithm, "LRU") == 0){
				string_append(&dump, "\tLRU: ");
				char* lru = string_itoa(partition->lru);
				string_append(&dump, lru);
				free(lru);
			}

			string_append(&dump, "\tID: ");
			char* ID = string_itoa(partition->ID_message);
			string_append(&dump, ID);
			free(ID);

			string_append(&dump, "\tCola: ");
			char* cola = queue_to_string(partition->queue_code);
			string_append(&dump, cola);
			//free(cola);	//no hace malloc

		}
	}


	string_append(&dump, "\n-----------------------------------------------------------------------------------------------------------------------------\n");

	FILE* file_dump = txt_open_for_append("dump.txt");
	txt_write_in_file(file_dump, dump);
	txt_close_file(file_dump);

	free(dump);
	log_info(logger, "dump.txt creado\n");
	log_info(obligatorio, "Dump creado");
}

char* queue_to_string(queue_code queue_code){
	switch(queue_code){
	case COLA_NEW:
		return "NEW_PK";
	case COLA_APPEARED:
		return "APPEARED_PK";
	case COLA_GET:
		return "GET_PK";
	case COLA_LOCALIZED:
		return "LOCALIZED_PK";
	case COLA_CATCH:
		return "CATCH_PK";
	case COLA_CAUGHT:
		return "CAUGHT_PK";
	default:
		return "No existe";

	}
}


//------------------------------------------------------------------------- Funciones de prueba

void imprimir_lista(t_list* lista, char* nombre){
	int tamanio = list_size(lista);
	int i;
	void* contenido;

	for(i=0; i<tamanio; i++){
		contenido = list_get(lista, i);
		printf("%s[%d] vale: %d\n", nombre, i, (int) contenido);
	}

}

void encontrar_numero(t_list *lista, void* numero_buscado, t_list* elementos_coincidentes) {
	bool _soy_el_numero_buscado(void *p) {
		return p == numero_buscado;
	}
	if(list_any_satisfy(lista, _soy_el_numero_buscado)){
		list_add(elementos_coincidentes, numero_buscado);
	}
}

t_list* interseccion_lista(t_list* unaLista, t_list* otraLista, t_list* resultado){
	list_clean(resultado);
	t_list* lista = list_duplicate(unaLista);
	void* elemento;

	while(list_size(lista) != 0){
		elemento = list_remove(lista, 0);
		encontrar_numero(otraLista, elemento, resultado);
	}

	if(list_size(lista) == 0)
		list_destroy(lista);
	else
		printf("ERROR no se vacio la lista\n");

	return resultado;
}


void no_contenido(t_list* lista_enviados, void* elemento_lista_global, t_list* resultado) {
	bool _soy_el_numero_buscado(void *p) {
		return p == elemento_lista_global;
	}
	if(!list_any_satisfy(lista_enviados, _soy_el_numero_buscado)){
		list_add(resultado, elemento_lista_global);
	}
}

t_list* no_enviados_lista(t_list* lista_global, t_list* lista_enviados, t_list** resultado){
	list_clean(*resultado);
	t_list* lista = list_duplicate(lista_global);
	void* elemento;

	while(list_size(lista) != 0){
		elemento = list_remove(lista, 0);
		no_contenido(lista_enviados, elemento, *resultado);
	}

	if(list_size(lista) == 0)
		list_destroy(lista);
	else
		printf("ERROR no se vacio la lista\n");

	return *resultado;
}


