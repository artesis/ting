<?php
class TingClient {
  protected $logger;

  public function __construct($logger)
  {
    $this->logger = $logger;
  }

  public function execute(TingRequestInterface $request)
  {
    $result = $this->makeRequest($request);

    $resultClass = $request->getResultClass();
    if ($resultClass) {
      return new $resultClass($result, $request);
    }
    return $result;
  }

  protected function makeRequest(TingRequestInterface $request) {
    // Parameters for the SOAP request
    $parameters = $request->getParameters();
    $action = $request->getAction();

    try {
      try {
        $startTime = microtime(TRUE);

        $client = new NanoSOAPClient($request->getUrl());
        $response = $client->call($action, $parameters);

        $stopTime = microtime(TRUE);
        $time = $stopTime - $startTime;

        $this->logger->log(
            'Completed SOAP request ' . $action . ' ' .
            $request->getUrl() .
            ' (' . round($time, 3) . 's). ' .
            'Request body: ' . $client->requestBodyString .
            ' Response: ' . $response
        );

        if ($request->processResults()) {
          if (empty($parameters['outputType']) || $parameters['outputType'] == 'xml') {
            $result = simplexml_load_string($response);
          }
          else {
            switch ($parameters['outputType']) {
              case 'json' :
                $result = new JsonOutput($response);
                break;
              case 'php' :
                $result = unserialize($response);
            }
          }
          return $result;
        }

        return $response;
      }
      catch (NanoSOAPcURLException $e) {
        //Convert NanoSOAP exceptions to TingClientExceptions as callers
        //should not deal with protocol details
        throw new TingClientException($e->getMessage(), $e->getCode());
      }
    }
    catch (TingClientException $e) {
      $this->logger->log('Error handling SOAP request ' . $action . ' ' .
          $request->getUrl() .': '. $e->getMessage()
      );
      throw $e;
    }
  }
}
