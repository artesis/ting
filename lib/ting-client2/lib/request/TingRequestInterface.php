<?php
interface TingRequestInterface {
  public function __construct($url, $agency, $profile);
  public function setUrl($url);
  public function getUrl();
  public function setParameter($name, $value);
  public function getParameter($name);
  public function getParameters();
  public function getResultClass();
  public function setResultClass($class);

  /**
   * Tell if result must be processed by OutputInterface.
   */
  public function processResults();
}
