#!/usr/bin/env python3
import grpc
from concurrent import futures
import centralStorage_pb2
import centralStorage_pb2_grpc
import sys
import threading

class CentralRegistryServicer(centralStorage_pb2_grpc.CentralRegistryServicer):
    def __init__(self,stop_event):
        self._stop_event = stop_event
        self.key_server_mapping = {}
        self.mutex = threading.Lock()

    #procedimento que recebe um identificador de servidor e uma lista de chaves e armazena no servidor central
    def Register(self, request, context):
        processed_keys = 0
        with self.mutex:
            server_id = request.server_id
            for key in request.keys:
                
                if key not in self.key_server_mapping or self.key_server_mapping[key] != server_id:
                    self.key_server_mapping[key] = server_id
                    processed_keys += 1

            return centralStorage_pb2.RegisterResponse(processed_keys=processed_keys)

    #procedimento que recebe uma chave e retorna o identificador do servidor que a armazena
    def MapKey(self, request, context):
        with self.mutex:
            key = request.key
            if key in self.key_server_mapping:
                server_id = self.key_server_mapping[key]
                return centralStorage_pb2.MappingResponse(server_id=server_id)
            else:
                return centralStorage_pb2.MappingResponse()

    #procedimento que finaliza o servidor central
    def Terminate(self, request, context):
        self._stop_event.set()
        return centralStorage_pb2.TerminateResponse(result = self.processed_keys)

if __name__ == "__main__":
    port = sys.argv[1]
    #Evento para tornar possivel finalizar o servidor pelo cliente
    stop_event = threading.Event()
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    centralStorage_pb2_grpc.add_CentralRegistryServicer_to_server(
        CentralRegistryServicer(stop_event), server
    )
    server.add_insecure_port(f'0.0.0.0:{port}')
    server.start()
    stop_event.wait()
    server.stop(0)