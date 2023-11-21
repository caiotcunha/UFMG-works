import grpc
from concurrent import futures
import storage_pb2
import storage_pb2_grpc
import sys

class KeyValueStoreServicer(storage_pb2_grpc.KeyValueStoreServicer):
    def __init__(self,stop_event,flag = False):
        self.flag = flag
        self.key_value_dict = {}
        self.stop_event = stop_event

    def Insert(self, request, context):
        key = request.key
        value = request.value
        if key in self.key_value_dict:
            return storage_pb2.InsertResponse(result=-1)
        else:
            self.key_value_dict[key] = value
            return storage_pb2.InsertResponse(result=0)

    def Query(self, request, context):
        key = request.key
        if key in self.key_value_dict:
            return storage_pb2.QueryResponse(value=self.key_value_dict[key])
        else:
            return storage_pb2.QueryResponse()

    def Activate(self, request, context):
        if(self.flag):
             #Implemente a lógica de ativação aqui
            return storage_pb2.ActivationResponse(value = 0)
        else:
            print(flag)
            return storage_pb2.ActivationResponse(value = 0)

    def Terminate(self, request, context):
        # Implemente a lógica de término aqui.
        self._stop_event.set()
        return storage_pb2.TerminateResponse(result=0)

def serve(port,flag):
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    storage_pb2_grpc.add_KeyValueStoreServicer_to_server(KeyValueStoreServicer(flag), server)
    server.add_insecure_port(port)
    server.start()
    server.wait_for_termination()

if __name__ == '__main__':
    port = sys.argv[1]
    flag = False
    
    if len(sys.argv) == 3:
        flag = True
    
    serve(port,flag)