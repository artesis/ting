<?php

class TingSearchRequest extends TingGenericRequest {

  protected $resultClass = 'TingSearchResult';

  public function __construct($url, $agency, $profile) {
    parent::__construct($url, $agency, $profile);
    $this->setLimits(1, 10);
    $this->setParameter('collectionType', 'manifestation');
    $this->setParameter('facets', array('facetName' => array(),'numberOfTerms' => 0));
    $this->setParameter('allRelations', true);
    $this->setParameter('relationData', 'full');
  }

  public function skipRelations() {
    $this->unsetParameter('allRelations');
    $this->unsetParameter('relationData');
  }

  public function setLimits($start, $perPage) {
    $this->setParameter('start', $start);
    $this->setParameter('stepValue', $perPage);
  }

  public function setQuery($query) {
    $this->setParameter('query', $query);
  }

  public function getQuery() {
    return $this->getParameter('query');
  }

  public function setFacets($facets) {
    $_facets = $this->getParameter('facets');
    $_facets['facetName'] = $facets;
    $this->setParameter('facets', $_facets);
  }

  public function getFacets() {
    $_facets = $this->getParameter('facets');
    return $_facets['facetName'];
  }

  /**
   * Set number of terms to show in facets.
   * @param int $terms
   */
  public function setTermsInFacets($terms) {
    $_facets = $this->getParameter('facets');
    $_facets['numberOfTerms'] = $terms;
    $this->setParameter('facets', $_facets);
  }

  public function getTermsInFacets() {
    $_facets = $this->getParameter('facets');
    return $_facets['numberOfTerms'];
  }

  public function setRank($value) {
    $this->setParameter('rank', $value);
  }

  public function getRank() {
    return $this->getParameter('rank');
  }

  public function setSort($value) {
    $this->setParameter('sort', $value);
  }

  public function getSort() {
    return $this->getParameter('sort');
  }

}
