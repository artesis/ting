<?php

class TingRelatedObjectsRequest extends TingGenericRequest {

  protected $resultClass = 'TingRelatedObjectsResult';

  public function __construct($url, $agency, $profile) {
    parent::__construct($url, $agency, $profile);
    $this->setLimits(1, 10);
    $this->setParameter('allObjects', true);
  }

  public function setQuery($query) {
    $this->setParameter('query', $query);
  }

  public function getQuery() {
    return $this->getParameter('query');
  }

  public function setLimits($start, $perPage) {
    $this->setParameter('start', $start);
    $this->setParameter('stepValue', $perPage);
  }

}
