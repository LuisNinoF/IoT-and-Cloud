import json
import boto3
from decimal import Decimal
from boto3.dynamodb.conditions import Key

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
        #Get the device_id from the query string parameters
        #device_id = event['queryStringParameters']['device_id']
        device_id = event['pathParameters']['device_id']
        #Query the DynamoDB table for the device data
        response = table.query(
            KeyConditionExpression=Key('device_id').eq(device_id),
            ScanIndexForward=False,                                 #get latest item
            Limit=1                                                 #get only one item
            )
        
        if response['Items']:
            #Get the latest item from the response
            latest_item = response['Items'][0]
            #Return the data with a 200 status code
            return {
                'statusCode': 200,
                'body': json.dumps(latest_item, cls=DecimalEncoder)
            }
        else:
            #Return a 404 status code if no data is found
            return {
                'statusCode': 404,
                'body': json.dumps({'error': 'No data found for the specified device_id'})
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
            'statusCode': 500,
            'body': json.dumps({'error': str(e)})
        }
