#!/usr/bin/env python3
import grpc
from concurrent import futures
import storage_pb2
import storage_pb2_grpc
import centralStorage_pb2
import centralStorage_pb2_grpc
import sys
import threading
import socket
class KeyValueStoreServicer(storage_pb2_grpc.KeyValueStoreServicer):
    def __init__(self,stop_event,port,flag = False):
        self._stop_event = stop_event
        self.flag = flag
        self.key_value_dict = {}
        self.central = 0
        self.port = port

    #procedimento que recebe uma chave e um valor e armazena no servidor
    def Insert(self, request, context):
        key = request.key
        value = request.value
        if key in self.key_value_dict:
            return storage_pb2.InsertResponse(result=-1)
        else:
            self.key_value_dict[key] = value
            return storage_pb2.InsertResponse(result=0)

    #procedimento que recebe uma chave e retorna o valor armazenado no servidor
    def Query(self, request, context):
        key = request.key
        if key in self.key_value_dict:
            return storage_pb2.QueryResponse(value=self.key_value_dict[key])
        else:
            return storage_pb2.QueryResponse(value = "")

    #procedimento que recebe um string identificador de um serviço e retorna o número de chaves armazenadas no servidor
    def Activate(self, request, context):
        if(self.flag):
            #recebe como parâmetro um string identificador de um serviço
            service = request.service_id
            #conecta-se ao servidor centralizador
            channel  = grpc.insecure_channel(service)
            self.central = centralStorage_pb2_grpc.CentralRegistryStub(channel)
            # faz uma chamada do procedimento de registro daquele servidor
            # primeiro parâmetro: o string de identificação dele mesmo
            id_string = socket.getfqdn()
            id_server = f"{id_string}:{self.port}"
            # segundo parâmetro: lista com todas as chaves que já foram inseridas até o momento
            keys_to_register = list(self.key_value_dict.keys())
            response = self.central.Register(centralStorage_pb2.ServerInfo(server_id=id_server, keys=keys_to_register))
            return storage_pb2.ActivationResponse(value = response.processed_keys)
        else:
            return storage_pb2.ActivationResponse(value = 0)

    #procedimento que finaliza o servidor
    def Terminate(self, request, context):
        self._stop_event.set()
        return storage_pb2.TerminateResponse(result=0)

if __name__ == '__main__':
    port = sys.argv[1]
    flag = False
    #testa se tem a flag de ativação
    if len(sys.argv) == 3:
        flag = True
    #Evento para tornar possivel finalizar o servidor pelo cliente
    stop_event = threading.Event()
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    storage_pb2_grpc.add_KeyValueStoreServicer_to_server(KeyValueStoreServicer(stop_event,port,flag), server)
    server.add_insecure_port(f'0.0.0.0:{port}')
    server.start()
    stop_event.wait()
    server.stop(0)