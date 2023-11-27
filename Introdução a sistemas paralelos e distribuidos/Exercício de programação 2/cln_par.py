#!/usr/bin/env python3
import grpc
import storage_pb2
import storage_pb2_grpc
import sys

if __name__ == '__main__':
    port = sys.argv[1]
    channel = grpc.insecure_channel(port)
    stub = storage_pb2_grpc.KeyValueStoreStub(channel)
    while True:
        try:
            userInput = input()
            comandList = userInput.split(',')
            if comandList[0] == 'I':
                response = stub.Insert(storage_pb2.KeyValue(key=int(comandList[1]), value=comandList[2]))
                print(response.result)
            elif comandList[0] == 'C':
                response = stub.Query(storage_pb2.Key(key=int(comandList[1])))
                print(response.value)
            elif comandList[0] == 'A':
                response = stub.Activate(storage_pb2.ActivationRequest(service_id=comandList[1]))
                print(response.value)
            elif comandList[0] == 'T':
                response = stub.Terminate(storage_pb2.TerminateParams())
                print(response.result)
                break
            elif comandList[0] == 'exit':
                break
        except EOFError:
            break
        
        #run()