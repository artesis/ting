<?php
class TingGenericRequest implements TingRequestInterface {
  protected $url;
  protected $parameters = array();
  protected $action = 'searchRequest';
  protected $resultClass = NULL;
  protected $rawResults = FALSE;

  /**
   * Generic request.
   *
   * @param string $url
   *   URL of the webservice.
   * @param string $agency
   *   Agency ID
   * @param string $profile
   *   Profile name
   */
  public function __construct($url, $agency, $profile) {
    $this->setUrl($url);
    $this->setAgency($agency);
    $this->setProfile($profile);
    $this->setOutputType('json');
  }

  public function setAgency($agency) {
    $this->setParameter('agency', $agency);
  }
  public function getAgency() {
    return $this->getParameter('agency');
  }
  public function setProfile($profile) {
    $this->setParameter('profile', $profile);
  }
  public function getProfile() {
    $this->getParameter('profile');
  }
  public function setUrl($url) {
    $this->url = $url;
  }
  public function getUrl() {
    return $this->url;
  }
  public function setParameter($name, $value) {
    $this->parameters[$name] = $value;
  }
  public function getParameter($name) {
    return isset($this->parameters[$name]) ? $this->parameters[$name] : NULL;
  }
  public function unsetParameter($name) {
    if (isset($this->parameters[$name])) {
      unset($this->parameters[$name]);
    }
  }
  public function getResultClass() {
    return $this->resultClass;
  }
  public function setResultClass($class) {
    $this->resultClass = $class;
  }
  public function getParameters() {
    return $this->parameters;
  }
  public function getAction() {
    return $this->action;
  }

  /**
   *
   * @param string $type
   *   Values: xml, json, php
   *   Default: json
   */
  public function setOutputType($type) {
    $this->setParameter('outputType', $type);
  }

  /**
   * Tell request to skip processing results.
   */
  public function skipProcessingResult() {
    $this->resultClass = NULL;
    $this->rawResults = TRUE;
  }

  /**
   * (non-PHPdoc)
   * @see TingRequestInterface::processResults()
   */
  public function processResults() {
    return !$this->rawResults;
  }
}
