import json
import boto3
from decimal import Decimal
from datetime import datetime

#Initialize DynamoDB resource and table
dynamodb = boto3.resource('dynamodb')
table = dynamodb.Table('DeviceData')

#Custom JSON encoder to handle Decimal type
class DecimalEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, Decimal):
            return float(obj)
        return super(DecimalEncoder, self).default(obj)

def lambda_handler(event, context):
    try:
        # Parse the JSON data from the event
        body = json.loads(event['body'])
        device_id = body['device_id']
        temperature = body['temperature']
        humidity = body['humidity']
        if 'timestamp' in body:
            timestamp = body['timestamp']
        else:
            timestamp = int(datetime.now().timestamp())

        #Update device data in DynamoDB based on device_id and timestamp
        response = table.update_item(
            Key={
                'device_id': device_id,
                'timestamp': timestamp
            },
            UpdateExpression='SET temperature = :t, humidity = :h',
            ExpressionAttributeValues={
                ':t': Decimal(str(temperature)),
                ':h': Decimal(str(humidity))
            },
            ReturnValues='UPDATED_NEW'
        )

        #Return a success response
        return {
            'statusCode': 200,
            'body': json.dumps({'message': 'Device data stored/updated successfully'})
        }
    
    except KeyError as e:
        #Handle missing required fields
        return {
            'statusCode': 400,
            'body': json.dumps({'error': f'Missing required key: {str(e)}'})
        }
    
    except Exception as e:
        #Handle any other exceptions
        return {
            'statusCode': 400,
            'body': json.dumps({'error': str(e)})
        }
