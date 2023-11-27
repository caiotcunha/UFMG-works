#!/usr/bin/env python3
import grpc
import centralStorage_pb2
import centralStorage_pb2_grpc
import sys

if __name__ == '__main__':
    port = sys.argv[1]
    channel = grpc.insecure_channel(port)
    stub = centralStorage_pb2_grpc.CentralRegistryStub(channel)
    while True:
        try:
            userInput = input()
            comandList = userInput.split(',')
            if comandList[0] == 'C':
                response = stub.MapKey(centralStorage_pb2.KeyRequest(key=int(comandList[1])))
                print(response.server_id)
            elif comandList[0] == 'T':
                response = stub.Terminate(centralStorage_pb2.Empty())
                print(response.result)
                break
        except EOFError:
            break