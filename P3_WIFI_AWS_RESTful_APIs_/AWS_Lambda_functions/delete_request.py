import json
import boto3

#Initialize DynamoDB resource and table
dynamodb = boto3.resource('dynamodb')
table = dynamodb.Table('DeviceData')

def lambda_handler(event, context):
    device_id = event['pathParameters']['device_id']
    timestamp = int(event['queryStringParameters']['timestamp'])
    
    table.delete_item(
        Key={
            'device_id': device_id,
            'timestamp': timestamp
        }
    )

    return {
        'statusCode': 200,
        'body': json.dumps({'message':'Data deleted successfully'})
    }



