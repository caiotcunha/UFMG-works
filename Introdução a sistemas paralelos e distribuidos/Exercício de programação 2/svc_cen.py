import grpc
from concurrent import futures
import centralStorage_pb2
import centralStorage_pb2_grpc
import sys
import threading

class CentralRegistryServicer(centralStorage_pb2_grpc.CentralRegistryServicer):
    def __init__(self,stop_event):
        self.processed_keys = 0
        self._stop_event = stop_event
        self.key_server_mapping = {}

    def Register(self, request, context):
        server_id = request.server_id

        for key in request.keys:
            self.key_server_mapping[key] = server_id
            self.processed_keys += 1

        return centralStorage_pb2.RegisterResponse(processed_keys=self.processed_keys)

    def MapKey(self, request, context):
        key = request.key
        if key in self.key_server_mapping:
            server_id = self.key_server_mapping[key]
            return centralStorage_pb2.MappingResponse(server_id=server_id)
        else:
            return centralStorage_pb2.MappingResponse()

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
    server.add_insecure_port(port)
    server.start()
    stop_event.wait()
    server.stop(0)