import json
import boto3
from datetime import datetime

# Gives access to DynamoDB
dynamodb = boto3.resource('dynamodb')
# References desired table to a variable in the script
table = dynamodb.Table('DeviceData')

def lambda_handler(event,context):
    try:
        # Parse the JSON data from the event
        body = json.loads(event['body'])
        device_id = body['device_id']
        temperature = body['temperature']
        humidity = body['humidity']
        timestamp = int(datetime.now().timestamp())

        # Store the data in DynamoDB
        table.put_item(
            Item={
                'device_id': device_id,
                'timestamp': timestamp,
                'temperature': temperature,
                'humidity': humidity
            }
        )

        # Return success
        return {
            'statusCode': 200,
            'body': json.dumps({'message':'Data stored successfully'})
        }

    #Exception if not successful
    except Exception as e:
        # Return error
        return {
            'statusCode': 400,
            'body': json.dumps({'error': str(e)})
        }
